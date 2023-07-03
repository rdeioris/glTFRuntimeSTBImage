// Copyright 2020-2022, Roberto De Ioris.

#include "glTFRuntimeSTBImage.h"

#include "glTFRuntimeParser.h"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#include "stb_image.h"


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
}

void FglTFRuntimeSTBImageModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FglTFRuntimeSTBImageModule, glTFRuntimeSTBImage)