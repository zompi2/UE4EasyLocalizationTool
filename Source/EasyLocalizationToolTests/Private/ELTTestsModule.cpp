// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY_STATIC(ELTTestsLog, Log, All);

/**
 * Easy Localization Tool Tests Module
 * 
 * Provides comprehensive automated tests using Unreal's Automation Framework.
 * Tests validate the GenerateLocFilesImpl functionality with various CSV formats:
 * - Valid CSVs with different namespace configurations
 * - Invalid CSVs with structural errors
 * - Localization file generation and verification
 * - DevNotes support and string table generation
 * 
 * Run tests via:
 * - Window > Developer Tools > Automation
 * - Filter by "EasyLocalizationTool" or specific test name
 * - Click "Start Tests"
 */
class FEasyLocalizationToolTestsModule : public IModuleInterface
{
public:

	virtual void StartupModule() override
	{
		UE_LOG(ELTTestsLog, Log, TEXT("EasyLocalizationToolTests module loaded."));
		UE_LOG(ELTTestsLog, Log, TEXT("Use Window > Developer Tools > Automation to run tests."));
		UE_LOG(ELTTestsLog, Log, TEXT("Filter by 'EasyLocalizationTool' to find all tests."));
	}

	virtual void ShutdownModule() override
	{
		UE_LOG(ELTTestsLog, Log, TEXT("EasyLocalizationToolTests module unloaded."));
	}
};

IMPLEMENT_MODULE(FEasyLocalizationToolTestsModule, EasyLocalizationToolTests);
