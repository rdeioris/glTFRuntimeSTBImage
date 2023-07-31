// Copyright 2020-2023, Roberto De Ioris.

#include "glTFRuntimeSTBImage.h"

#include "glTFRuntimeParser.h"

THIRD_PARTY_INCLUDES_START

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#include "stb_image.h"

#if defined(GLTFRUNTIME_IMAGE_API_1)
#define STB_DXT_IMPLEMENTATION
#include "stb_dxt.h"
#endif

THIRD_PARTY_INCLUDES_END


#define LOCTEXT_NAMESPACE "FglTFRuntimeSTBImageModule"

void FglTFRuntimeSTBImageModule::StartupModule()
{
#if defined(GLTFRUNTIME_IMAGE_API_1)
	FglTFRuntimeParser::OnTexturePixels.AddLambda([](TSharedRef<FglTFRuntimeParser> Parser, TSharedRef<FJsonObject> JsonImageObject, TArray64<uint8>& CompressedPixels, int32& Width, int32& Height, EPixelFormat& PixelFormat, TArray64<uint8>& UncompressedPixels, const FglTFRuntimeImagesConfig& ImagesConfig)

#else
	FglTFRuntimeParser::OnTexturePixels.AddLambda([](TSharedRef<FglTFRuntimeParser> Parser, TSharedRef<FJsonObject> JsonImageObject, TArray64<uint8>& CompressedPixels, int32& Width, int32& Height, TArray64<uint8>& UncompressedPixels)
#endif
		{
			// skip if already processed
			if (UncompressedPixels.Num() > 0)
			{
				return;
			}

	int32 ChannelsInFile;
#if defined(GLTFRUNTIME_IMAGE_API_1)
	if (stbi_is_hdr_from_memory(CompressedPixels.GetData(), CompressedPixels.Num()))
	{
		PixelFormat = EPixelFormat::PF_FloatRGBA;
		float* Pixels = stbi_loadf_from_memory(CompressedPixels.GetData(), CompressedPixels.Num(), &Width, &Height, &ChannelsInFile, 4);
		if (Pixels)
		{
			const int64 ImageSize = Width * Height * 4;
			UncompressedPixels.AddUninitialized(ImageSize * sizeof(uint16));
			for (int64 Index = 0; Index < ImageSize; Index++)
			{
				FFloat16 Color16(Pixels[Index]);
				FMemory::Memcpy(UncompressedPixels.GetData() + (Index * sizeof(uint16)), &(Color16.Encoded), sizeof(uint16));
			}
			stbi_image_free(Pixels);
		}
	}
	else
	{
		PixelFormat = EPixelFormat::PF_B8G8R8A8;
#endif
		uint8* Pixels = stbi_load_from_memory(CompressedPixels.GetData(), CompressedPixels.Num(), &Width, &Height, &ChannelsInFile, 4);
		if (Pixels)
		{
			// RGBA to BGRA
			const int64 ImageSize = Width * Height * 4;
			for (int64 IndexR = 0; IndexR < ImageSize; IndexR += 4)
			{
				Swap(Pixels[IndexR], Pixels[IndexR + 2]);
			}
			UncompressedPixels.Append(Pixels, ImageSize);
			stbi_image_free(Pixels);
		}
#if defined(GLTFRUNTIME_IMAGE_API_1)
	}
#endif
		});

#if defined(GLTFRUNTIME_IMAGE_API_1)
	FglTFRuntimeParser::OnTextureFilterMips.AddLambda([](TSharedRef<FglTFRuntimeParser> Parser, TArray<FglTFRuntimeMipMap>& Mips, const FglTFRuntimeImagesConfig& ImagesConfig)
		{
			if (!ImagesConfig.bCompressMips)
			{
				return;
			}

	for (FglTFRuntimeMipMap& Mip : Mips)
	{
		if (Mip.PixelFormat == EPixelFormat::PF_B8G8R8A8)
		{
			EPixelFormat CompressedFormat = EPixelFormat::PF_DXT5;
			if (ImagesConfig.Compression == TextureCompressionSettings::TC_Normalmap)
			{
				CompressedFormat = EPixelFormat::PF_BC5;
			}

			const int64 BlockX = GPixelFormats[CompressedFormat].BlockSizeX;
			const int64 BlockY = GPixelFormats[CompressedFormat].BlockSizeY;
			const int64 BlockBytes = GPixelFormats[Mip.PixelFormat].BlockBytes;
			const int64 MipWidthAligned = FMath::Max(((Mip.Width / BlockX) + ((Mip.Width % BlockX) != 0 ? 1 : 0)) * BlockX, BlockX);
			const int64 MipHeightAligned = FMath::Max(((Mip.Height / BlockY) + ((Mip.Height % BlockY) != 0 ? 1 : 0)) * BlockY, BlockY);
			const int64 MipWantedSize = (MipWidthAligned * BlockBytes * MipHeightAligned);
			const int64 MipNewSize = (MipWidthAligned * GPixelFormats[CompressedFormat].BlockBytes * MipHeightAligned) / (BlockX * BlockY);

			// avoid reading out of the original pixels data
			if (Mip.Pixels.Num() < MipWantedSize)
			{
				Mip.Pixels.AddUninitialized(MipWantedSize - Mip.Pixels.Num());
			}

			const int64 Pitch = Mip.Width * BlockBytes;

			TArray<uint8> OutPixels;
			OutPixels.AddUninitialized(MipNewSize);

			uint8* OutPtr = OutPixels.GetData();

			for (int32 PixelY = 0; PixelY < Mip.Height; PixelY += 4)
			{
				for (int32 PixelX = 0; PixelX < Mip.Width; PixelX += 4)
				{
					TArray<uint8, TInlineAllocator<64>> Block;
					Block.AddUninitialized(64);
					FMemory::Memcpy(&Block, &Mip.Pixels[(PixelY + 0) * Pitch + PixelX * BlockBytes], BlockBytes * 4);
					FMemory::Memcpy(&Block[16], &Mip.Pixels[(PixelY + 1) * Pitch + PixelX * BlockBytes], BlockBytes * 4);
					FMemory::Memcpy(&Block[32], &Mip.Pixels[(PixelY + 2) * Pitch + PixelX * BlockBytes], BlockBytes * 4);
					FMemory::Memcpy(&Block[48], &Mip.Pixels[(PixelY + 3) * Pitch + PixelX * BlockBytes], BlockBytes * 4);

					// swap R & B
					for (int32 ByteIndex = 0; ByteIndex < 64; ByteIndex += 4)
					{
						Swap(Block[ByteIndex], Block[ByteIndex + 2]);
					}

					if (CompressedFormat == EPixelFormat::PF_BC5)
					{
						TArray<uint8, TInlineAllocator<32>> RGBlock;
						for (int32 ByteIndex = 0; ByteIndex < 64; ByteIndex += 4)
						{
							RGBlock.Append(&Block[ByteIndex], 2);
						}
						stb_compress_bc5_block(OutPtr, RGBlock.GetData());
					}
					else
					{

						stb_compress_dxt_block(OutPtr, Block.GetData(), 1, STB_DXT_HIGHQUAL);
					}
					OutPtr += 16;
				}
			}

			Mip.PixelFormat = CompressedFormat;
			Mip.Pixels = OutPixels;
		}
	}
		}
	);
#endif
}

void FglTFRuntimeSTBImageModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FglTFRuntimeSTBImageModule, glTFRuntimeSTBImage)