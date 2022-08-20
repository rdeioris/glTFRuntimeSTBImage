// Copyright 2020-2022, Roberto De Ioris.

#include "glTFRuntimeSTBImage.h"

#include "glTFRuntimeParser.h"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#include "stb_image.h"


#define LOCTEXT_NAMESPACE "FglTFRuntimeSTBImageModule"

void FglTFRuntimeSTBImageModule::StartupModule()
{

	FglTFRuntimeParser::OnTexturePixels.AddLambda([](TSharedRef<FglTFRuntimeParser> Parser, TSharedRef<FJsonObject> JsonImageObject, TArray64<uint8>& CompressedPixels, int32& Width, int32& Height, TArray64<uint8>& UncompressedPixels)
		{
			// skip if already processed
			if (UncompressedPixels.Num() > 0)
			{
				return;
			}
			int32 ChannelsInFile;
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
		});
}

void FglTFRuntimeSTBImageModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FglTFRuntimeSTBImageModule, glTFRuntimeSTBImage)