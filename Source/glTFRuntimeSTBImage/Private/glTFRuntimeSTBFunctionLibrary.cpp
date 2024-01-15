// Copyright 2020-2024, Roberto De Ioris.

#include "glTFRuntimeSTBFunctionLibrary.h"
#include "glTFRuntimeSTBImage.h"

UTexture2DArray* UglTFRuntimeSTBFunctionLibrary::LoadAnimGIFAsTexture2DArray(UglTFRuntimeAsset* Asset, int32& NumFrames, float& FPS, const FglTFRuntimeImagesConfig& ImagesConfig)
{
	TArray<FglTFRuntimeMipMap> Mips;
	float Duration = 0;
	if (!FglTFRuntimeSTBImageModule::LoadAnimGIFFromBlob(Asset->GetParser()->GetBlob().GetData(), Asset->GetParser()->GetBlob().Num(), Mips, Duration))
	{
		return nullptr;
	}

	NumFrames = Mips.Num();
	FPS = NumFrames / Duration;

	return Asset->GetParser()->BuildTextureArray(Asset, Mips, ImagesConfig, FglTFRuntimeTextureSampler());
}