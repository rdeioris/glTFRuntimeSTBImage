// Copyright 2020-2022, Roberto De Ioris.

#pragma once

#include "CoreMinimal.h"
#include "glTFRuntimeImageLoader.h"
#include "glTFRuntime.h"
#include "glTFRuntimeImageLoaderSTBImage.generated.h"

/**
 * 
 */
UCLASS()
class GLTFRUNTIMESTBIMAGE_API UglTFRuntimeImageLoaderSTBImage : public UglTFRuntimeImageLoader
{
	GENERATED_BODY()
public:
	bool LoadImage(TSharedRef<FglTFRuntimeParser> Parser, const int32 ImageIndex, TSharedRef<FJsonObject> JsonImageObject, const TArray64<uint8>& Data, TArray64<uint8>& OutData, int32& Width, int32& Height) override;
	
};
