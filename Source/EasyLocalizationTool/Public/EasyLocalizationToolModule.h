// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "ELTCustomTextResource.h"

class EASYLOCALIZATIONTOOL_API FEasyLocalizationToolModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	void StartupModule() override;
	void ShutdownModule() override;

	TSharedPtr<FELTCustomTextResource> CustomTextResource;

};