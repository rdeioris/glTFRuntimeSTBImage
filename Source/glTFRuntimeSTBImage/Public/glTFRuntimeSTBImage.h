// Copyright 2020-2024, Roberto De Ioris.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FglTFRuntimeSTBImageModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static bool LoadAnimGIFFromBlob(const uint8* Data, const int32 Size, TArray<FglTFRuntimeMipMap>& Mips, float& Duration);
};
