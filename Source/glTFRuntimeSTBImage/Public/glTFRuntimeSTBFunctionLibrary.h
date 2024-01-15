// Copyright 2020-2024, Roberto De Ioris.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "glTFRuntimeAsset.h"
#include "glTFRuntimeSTBFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class GLTFRUNTIMESTBIMAGE_API UglTFRuntimeSTBFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "glTFRuntime|STBImage")
	static UTexture2DArray* LoadAnimGIFAsTexture2DArray(UglTFRuntimeAsset* Asset, int32& NumFrames, float& FPS, const FglTFRuntimeImagesConfig& ImagesConfig);
	
};
