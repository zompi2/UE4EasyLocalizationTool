// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#include "EasyLocalizationToolModule.h"

IMPLEMENT_MODULE(FEasyLocalizationToolModule, EasyLocalizationTool)

void FEasyLocalizationToolModule::StartupModule()
{
	CustomTextResource = MakeShared<FELTCustomTextResource>();
	FTextLocalizationManager::Get().RegisterTextSource(CustomTextResource.ToSharedRef(), true);
}

void FEasyLocalizationToolModule::ShutdownModule()
{
	CustomTextResource.Reset();
}