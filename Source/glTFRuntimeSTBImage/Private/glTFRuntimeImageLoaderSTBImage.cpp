// Copyright 2020-2022, Roberto De Ioris.


#include "glTFRuntimeImageLoaderSTBImage.h"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#include "stb_image.h"

bool UglTFRuntimeImageLoaderSTBImage::LoadImage(TSharedRef<FglTFRuntimeParser> Parser, const int32 ImageIndex, TSharedRef<FJsonObject> JsonImageObject, const TArray64<uint8>& Data, TArray64<uint8>& OutData, int32& Width, int32& Height)
{
	int32 ChannelsInFile;
	uint8* Pixels = stbi_load_from_memory(Data.GetData(), Data.Num(), &Width, &Height, &ChannelsInFile, 4);
	if (Pixels)
	{
		// RGBA to BGRA
		const int64 ImageSize = Width * Height * 4;
		for (int64 IndexR = 0; IndexR < ImageSize; IndexR += 4)
		{
			Swap(Pixels[IndexR], Pixels[IndexR + 2]);
		}
		OutData.Append(Pixels, ImageSize);
		stbi_image_free(Pixels);
		return true;
	}
	return false;
}