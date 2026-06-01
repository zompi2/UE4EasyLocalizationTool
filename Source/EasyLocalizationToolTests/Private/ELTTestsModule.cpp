// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#include "Modules/ModuleManager.h"

class FEasyLocalizationToolTestsModule : public IModuleInterface
{
public:

	void StartupModule() override
	{}

	virtual void ShutdownModule() override
	{}
};

IMPLEMENT_MODULE(FEasyLocalizationToolTestsModule, EasyLocalizationToolTests);
