// Copyright (c) 2026 Crezetique. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityObject.h"
#include "Engine/StreamableManager.h"
#include "ELTEditorAuditTypes.h"
#include "ELTEditorAuditor.generated.h"

class UWidgetBlueprint;

UCLASS()
class EASYLOCALIZATIONTOOLEDITOR_API UELTEditorAuditor : public UEditorUtilityObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "ELT|Localization Audit")
	void AuditSelectedAssets();

	UFUNCTION(BlueprintCallable, Category = "ELT|Localization Audit")
	void AuditAssets(const TArray<FAssetData>& Assets);

	static TArray<FELTAssetAuditResult> RunAudit(const TArray<FAssetData>& Assets);

	// Held alive until async asset loading completes before running the audit.
	static TSharedPtr<FStreamableHandle> PendingLoadHandle;

private:
	static void AuditSingleAsset(UObject* Asset, const FAssetData& AssetData, FELTAssetAuditResult& OutResult);

	// Audits every FText reachable through the CDO property tree. This will recursively run through base classes and relevant properties.
	static void AuditClassProperties(void* ContainerPtr, UStruct* Struct, const FString& PathPrefix, FELTAssetAuditResult& OutResult);

	// Audits Blueprint class variables, function local variables, function parameter pins, and inline node pins.
	static void AuditBlueprintTextSources(UBlueprint* Blueprint, FELTAssetAuditResult& OutResult);

	// Audits FText properties on UMG widget components inside a Widget Blueprint's widget tree.
	static void AuditWidgetComponents(UWidgetBlueprint* WidgetBlueprint, FELTAssetAuditResult& OutResult);

	static void AuditText(const FText& InText, EELTAuditParameterType ParameterType, const FString& VariableNodeName, const FGuid& NodeGuid, const FString& GraphName, FELTAssetAuditResult& OutResult);
};