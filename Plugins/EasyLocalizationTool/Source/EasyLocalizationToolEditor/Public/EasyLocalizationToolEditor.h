// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class SDockTab;

class EASYLOCALIZATIONTOOLEDITOR_API FEasyLocalizationToolEditorModule : public IModuleInterface, public FGCObject
{
public:

	void StartupModule() override;
	void AddReferencedObjects(FReferenceCollector& Collector) override;

private:
	
	class UELTEditor* Editor;
	TWeakPtr<SDockTab> EditorTab;
	TSharedRef<SDockTab> SpawnEditor(const FSpawnTabArgs& Args);
	bool CanSpawnEditor(const FSpawnTabArgs& Args);
};
