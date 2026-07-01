// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#include "EasyLocalizationToolModule.h"
#include "ELTImporter.h"

IMPLEMENT_MODULE(FEasyLocalizationToolModule, EasyLocalizationTool)

ELT_PRAGMA_DISABLE_OPTIMIZATION

void FEasyLocalizationToolModule::StartupModule()
{
	CustomTextResource = MakeShared<FELTCustomTextResource>();
	FTextLocalizationManager::Get().RegisterTextSource(CustomTextResource.ToSharedRef(), true);
}

void FEasyLocalizationToolModule::ShutdownModule()
{
	CustomTextResource.Reset();
}

ELT_PRAGMA_ENABLE_OPTIMIZATION