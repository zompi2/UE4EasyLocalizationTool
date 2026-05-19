// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#include "ELTEditorAuditor.h"
#include "ELTBlueprintLibrary.h"
#include "ELTEditorAuditWidget.h"

#include "EditorUtilityLibrary.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/AssetManager.h"

#include "Internationalization/StringTableCore.h"
#include "Internationalization/StringTableRegistry.h"
#include "Internationalization/TextKey.h"
#include "Runtime/Launch/Resources/Version.h"

#include "Engine/Blueprint.h"
#include "StructUtils/UserDefinedStruct.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "K2Node_FunctionEntry.h"
#include "WidgetBlueprint.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Widget.h"

DEFINE_LOG_CATEGORY_STATIC(ELTAuditLog, Log, All);

namespace ELTAuditInternal
{
	static FString NodeDisplayName(const UEdGraphNode* Node)
	{
		return Node->GetNodeTitle(ENodeTitleType::ListView).ToString();
	}
}

/*static*/ void UELTEditorAuditor::AuditText(
	const FText&           InText,
	EELTAuditParameterType ParameterType,
	const FString&         VariableNodeName,
	const FGuid&           NodeGuid,
	const FString&         GraphName,
	FELTAssetAuditResult&  OutResult)
{
	// Skip if localized is set to false
	if (InText.IsCultureInvariant())
	{
		return;
	}

	FString Package, Namespace, Key, Source;
	UELTBlueprintLibrary::GetTextData(InText, Package, Namespace, Key, Source);

	auto AddIssue = [&](EELTAuditIssueType IssueType, FString LocalizedString = TEXT(""), bool bIsUsingStringTable = false)
	{
		FELTAuditIssue& Row       = OutResult.Issues.Emplace_GetRef();
		Row.AssetName             = OutResult.AssetData.AssetName.ToString();
		Row.Type                  = ParameterType;
		Row.VariableNodeName      = VariableNodeName;
		Row.Issue                 = IssueType;
		Row.Value                 = Source;
		Row.LocalizedString       = MoveTemp(LocalizedString);
		Row.Key                   = Key;
		Row.Namespace             = Namespace;
		Row.bIsUsingStringTable   = bIsUsingStringTable;
		Row.NodeGuid              = NodeGuid;
		Row.GraphName             = GraphName;
	};

	if (InText.IsEmpty())
	{
		AddIssue(EELTAuditIssueType::EmptyValue);
		return;
	}

	// Check for a broken String Table reference before valid localization check
	FName   TableId;
	FString TableKey;
	if (FTextInspector::GetTableIdAndKey(InText, TableId, TableKey))
	{
		Key       = TableKey;
		Namespace = TableId.ToString();

		FStringTableConstPtr Table = FStringTableRegistry::Get().FindStringTable(TableId);
		bool bKeyValid = Table.IsValid() && Table->FindEntry(TableKey).IsValid();
		if (bKeyValid == false)
		{
			AddIssue(EELTAuditIssueType::StringTableMissingKey, TEXT(""), true);
		}
		else
		{
			AddIssue(EELTAuditIssueType::None, InText.ToString(), true);
		}
		return;
	}

	// We assume key missing or key-value mismatched means the text was never set up to fetch localization strings via ELT
	if (Key.IsEmpty() || Key.Equals(Source, ESearchCase::CaseSensitive) == false)
	{
		AddIssue(EELTAuditIssueType::NotYetLocalized);
		return;
	}

	FText FoundText;
	bool  bFoundInTable;

#if ((ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION >= 5))
	bFoundInTable = FText::FindTextInLiveTable_Advanced(*Namespace, *Key, FoundText, &Source);
#else
	bFoundInTable = FText::FindText(*Namespace, *Key, FoundText, &Source);
#endif

	if (bFoundInTable)
	{
		AddIssue(EELTAuditIssueType::None, FoundText.ToString());
	}
	else
	{
		AddIssue(EELTAuditIssueType::InvalidLocalization);
	}
}

/*static*/ void UELTEditorAuditor::AuditClassProperties(
	void*                 ContainerPtr,
	UStruct*              Struct,
	const FString&        PathPrefix,
	FELTAssetAuditResult& OutResult)
{
	for (TFieldIterator<FProperty> It(Struct, EFieldIteratorFlags::IncludeSuper); It; ++It)
	{
		FProperty* Prop = *It;

		const FString PropName = Cast<UUserDefinedStruct>(Struct)
			? Prop->GetAuthoredName()
			: Prop->GetName();

		const FString CurrentPath = PathPrefix.IsEmpty() ? PropName : PathPrefix + TEXT(".") + PropName;

		if (const FTextProperty* TextProp = CastField<FTextProperty>(Prop))
		{
			AuditText(TextProp->GetPropertyValue_InContainer(ContainerPtr),
				EELTAuditParameterType::ClassVariable, CurrentPath, FGuid(), TEXT(""), OutResult);
		}
		else if (const FStructProperty* StructProp = CastField<FStructProperty>(Prop))
		{
			AuditClassProperties(StructProp->ContainerPtrToValuePtr<void>(ContainerPtr), StructProp->Struct, CurrentPath, OutResult);
		}
		else if (const FArrayProperty* ArrayProp = CastField<FArrayProperty>(Prop))
		{
			FScriptArrayHelper ArrayHelper(ArrayProp, ArrayProp->ContainerPtrToValuePtr<void>(ContainerPtr));
			for (int32 i = 0; i < ArrayHelper.Num(); i++)
			{
				const FString ElemPath = FString::Printf(TEXT("%s[%d]"), *CurrentPath, i);
				void*         ElemPtr  = ArrayHelper.GetRawPtr(i);

				if (const FTextProperty* InnerText = CastField<FTextProperty>(ArrayProp->Inner))
				{
					AuditText(InnerText->GetPropertyValue(ElemPtr),
						EELTAuditParameterType::ClassVariable, ElemPath, FGuid(), TEXT(""), OutResult);
				}
				else if (const FStructProperty* InnerStruct = CastField<FStructProperty>(ArrayProp->Inner))
				{
					AuditClassProperties(ElemPtr, InnerStruct->Struct, ElemPath, OutResult);
				}
			}
		}
		else if (const FMapProperty* MapProp = CastField<FMapProperty>(Prop))
		{
			FScriptMapHelper MapHelper(MapProp, MapProp->ContainerPtrToValuePtr<void>(ContainerPtr));
			for (int32 i = 0; i < MapHelper.GetMaxIndex(); i++)
			{
				if (MapHelper.IsValidIndex(i) == false) { continue; }

				const FString ElemPath = FString::Printf(TEXT("%s[%d].Value"), *CurrentPath, i);
				void*         ValPtr   = MapHelper.GetValuePtr(i);

				if (const FTextProperty* ValText = CastField<FTextProperty>(MapProp->ValueProp))
				{
					AuditText(ValText->GetPropertyValue(ValPtr),
						EELTAuditParameterType::ClassVariable, ElemPath, FGuid(), TEXT(""), OutResult);
				}
				else if (const FStructProperty* ValStruct = CastField<FStructProperty>(MapProp->ValueProp))
				{
					AuditClassProperties(ValPtr, ValStruct->Struct, ElemPath, OutResult);
				}
			}
		}
	}
}

/*static*/ void UELTEditorAuditor::AuditBlueprintTextSources(
	UBlueprint*           Blueprint,
	FELTAssetAuditResult& OutResult)
{
	if (Blueprint == nullptr)
	{
		return;
	}

	TArray<UEdGraph*> AllGraphs;
	Blueprint->GetAllGraphs(AllGraphs);

	for (UEdGraph* Graph : AllGraphs)
	{
		if (Graph == nullptr) { continue; }

		const FString GraphName = Graph->GetName();

		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (Node == nullptr) { continue; }

			if (UK2Node_FunctionEntry* EntryNode = Cast<UK2Node_FunctionEntry>(Node))
			{
				// Local variables are stored on the entry node
				for (const FBPVariableDescription& LocalVar : EntryNode->LocalVariables)
				{
					if (LocalVar.VarType.PinCategory != UEdGraphSchema_K2::PC_Text) { continue; }

					FText DefaultText;
					if (FTextStringHelper::ReadFromBuffer(*LocalVar.DefaultValue, DefaultText) && DefaultText.IsEmpty() == false)
					{
						AuditText(DefaultText, EELTAuditParameterType::FunctionVariable,
							FString::Printf(TEXT("%s::%s"), *GraphName, *LocalVar.VarName.ToString()),
							EntryNode->NodeGuid, GraphName, OutResult);
					}
				}

				// Parameter pins default values
				for (UEdGraphPin* Pin : EntryNode->Pins)
				{
					if (Pin == nullptr || Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Text) { continue; }
					if (Pin->DefaultTextValue.IsEmpty()) { continue; }

					AuditText(Pin->DefaultTextValue, EELTAuditParameterType::FunctionVariable,
						FString::Printf(TEXT("%s::%s"), *GraphName, *Pin->PinName.ToString()),
						EntryNode->NodeGuid, GraphName, OutResult);
				}

				continue;
			}

			// All other nodes: unconnected literal FText input pins.
			for (UEdGraphPin* Pin : Node->Pins)
			{
				if (Pin == nullptr)                                         { continue; }
				if (Pin->Direction           != EGPD_Input)                 { continue; }
				if (Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Text) { continue; }
				if (Pin->LinkedTo.Num()       > 0)                          { continue; }
				if (Pin->DefaultTextValue.IsEmpty())                        { continue; }

				AuditText(Pin->DefaultTextValue, EELTAuditParameterType::NodeParameter,
					ELTAuditInternal::NodeDisplayName(Node), Node->NodeGuid, GraphName, OutResult);
			}
		}
	}
}

/*static*/ void UELTEditorAuditor::AuditSingleAsset(
	UObject*              Asset,
	const FAssetData&     AssetData,
	FELTAssetAuditResult& OutResult)
{
	OutResult.AssetData = AssetData;

	if (UBlueprint* Blueprint = Cast<UBlueprint>(Asset))
	{
		// Covers function locals, parameter pins, and nodes.
		AuditBlueprintTextSources(Blueprint, OutResult);

		// Covers UMG components.
		if (UWidgetBlueprint* WidgetBlueprint = Cast<UWidgetBlueprint>(Blueprint))
		{
			AuditWidgetComponents(WidgetBlueprint, OutResult);
		}

		// Covers CDO 
		// (Note: Class Properties will not suffice for Blueprint Assets)
		UObject* CDO = Blueprint->GeneratedClass ? Blueprint->GeneratedClass->GetDefaultObject(false) : nullptr;
		if (CDO != nullptr)
		{
			for (const FBPVariableDescription& Var : Blueprint->NewVariables)
			{
				if (Var.VarType.PinCategory != UEdGraphSchema_K2::PC_Text) { continue; }

				FProperty* Prop = Blueprint->GeneratedClass->FindPropertyByName(Var.VarName);
				if (const FTextProperty* TextProp = CastField<FTextProperty>(Prop))
				{
					const FText& Val = TextProp->GetPropertyValue_InContainer(CDO);
					AuditText(Val, EELTAuditParameterType::ClassVariable,
						Var.VarName.ToString(), FGuid(), TEXT(""), OutResult);
				}
			}
		}
	}
	else if (UUserDefinedStruct* UserStruct = Cast<UUserDefinedStruct>(Asset))
	{
		if (const uint8* DefaultInstance = UserStruct->GetDefaultInstance())
		{
			AuditClassProperties(
				const_cast<uint8*>(DefaultInstance),
				UserStruct,
				TEXT(""),
				OutResult);
		}
	}
	else
	{
		// Non-Blueprint Asset, Audit Class Defaults
		AuditClassProperties(Asset, Asset->GetClass(), TEXT(""), OutResult);
	}
}

/*static*/ void UELTEditorAuditor::AuditWidgetComponents(
	UWidgetBlueprint*     WidgetBlueprint,
	FELTAssetAuditResult& OutResult)
{
	if (WidgetBlueprint == nullptr || WidgetBlueprint->WidgetTree == nullptr)
	{
		return;
	}

	WidgetBlueprint->WidgetTree->ForEachWidget([&](UWidget* Widget)
	{
		if (Widget == nullptr) { return; }

		const FString ComponentName = Widget->GetName();

		for (TFieldIterator<FTextProperty> PropIt(Widget->GetClass(), EFieldIteratorFlags::IncludeSuper); PropIt; ++PropIt)
		{
			const FTextProperty* TextProp = *PropIt;
			const FName          PropName = TextProp->GetFName();

			// ToolTipText is intentionally empty on most widgets, skip if unset.
			if (PropName == FName("ToolTipText"))
			{
				const FText& Val = TextProp->GetPropertyValue_InContainer(Widget);
				if (Val.IsEmpty() == false)
				{
					AuditText(Val, EELTAuditParameterType::WidgetComponent,
						FString::Printf(TEXT("%s.ToolTipText"), *ComponentName), FGuid(), TEXT(""), OutResult);
				}
				continue;
			}

			// Accessible text fields are only relevant if set to override them with custom accessibility.
			if (PropName == FName("AccessibleText") || PropName == FName("AccessibleSummaryText"))
			{
#if (ENGINE_MAJOR_VERSION >= 5)
				if (Widget->bOverrideAccessibleDefaults == false) { continue; }

				const bool bIsAccessible        = (PropName == FName("AccessibleText"))        && (Widget->AccessibleBehavior        == ESlateAccessibleBehavior::Custom);
				const bool bIsSummaryAccessible = (PropName == FName("AccessibleSummaryText")) && (Widget->AccessibleSummaryBehavior == ESlateAccessibleBehavior::Custom);

				if (bIsAccessible == false && bIsSummaryAccessible == false) { continue; }
#else
				// Accessible override fields not supported in UE4 — skip entirely.
				continue;
#endif
			}
			
			// CommonUI internal classification hidden field — not a localizable text value.
			if (PropName == FName("PaletteCategory")) { continue; }

			const FText& Val = TextProp->GetPropertyValue_InContainer(Widget);
			AuditText(Val, EELTAuditParameterType::WidgetComponent,
				FString::Printf(TEXT("%s.%s"), *ComponentName, *PropName.ToString()), FGuid(), TEXT(""), OutResult);
		}
	});
}

TSharedPtr<FStreamableHandle> UELTEditorAuditor::PendingLoadHandle;

/*static*/ TArray<FELTAssetAuditResult> UELTEditorAuditor::RunAudit(const TArray<FAssetData>& Assets)
{
	TArray<FELTAssetAuditResult> Results;

	for (const FAssetData& AssetData : Assets)
	{
		UObject* Asset = AssetData.GetAsset();
		if (Asset == nullptr) { continue; }

		FELTAssetAuditResult& Result = Results.Emplace_GetRef();
		AuditSingleAsset(Asset, AssetData, Result);
	}

	UE_LOG(ELTAuditLog, Log, TEXT("Localization audit complete. %d asset(s) scanned."), Assets.Num());
	return Results;
}

static void RunAuditAndShow(const TArray<FAssetData>& Assets)
{
	const TArray<FELTAssetAuditResult> Results = UELTEditorAuditor::RunAudit(Assets);
	UELTEditorAuditWidget::ShowResults(Results);
}

static void RequestLoadThenAudit(const TArray<FAssetData>& Assets)
{
	// Cancel any in-progress load — the new request supersedes it.
	if (UELTEditorAuditor::PendingLoadHandle.IsValid())
	{
		UELTEditorAuditor::PendingLoadHandle->CancelHandle();
		UELTEditorAuditor::PendingLoadHandle.Reset();
	}

	TArray<FAssetData> Unloaded;
	for (const FAssetData& AssetData : Assets)
	{
		if (AssetData.IsAssetLoaded() == false)
		{
			Unloaded.Add(AssetData);
		}
	}

	if (Unloaded.IsEmpty())
	{
		RunAuditAndShow(Assets);
		return;
	}

	// Build soft object paths for all unloaded assets and request an async load.
	// PendingLoadHandle keeps the request alive until the callback fires.
	TArray<FSoftObjectPath> PathsToLoad;
	for (const FAssetData& AssetData : Unloaded)
	{
		PathsToLoad.Add(AssetData.ToSoftObjectPath());
	}

	UE_LOG(ELTAuditLog, Log, TEXT("Waiting for %d asset(s) to load before auditing..."), Unloaded.Num());

	UELTEditorAuditor::PendingLoadHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
		PathsToLoad,
		FStreamableDelegate::CreateLambda([Assets]()
		{
			UELTEditorAuditor::PendingLoadHandle.Reset();
			RunAuditAndShow(Assets);
		})
	);
}

void UELTEditorAuditor::AuditSelectedAssets()
{
	TArray<FAssetData> Assets = UEditorUtilityLibrary::GetSelectedAssetData();

#if (ENGINE_MAJOR_VERSION >= 5)
	const TArray<FString> SelectedPaths = UEditorUtilityLibrary::GetSelectedFolderPaths();
	if (SelectedPaths.Num() > 0)
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

		FARFilter Filter;
		Filter.bRecursivePaths = true;
		for (const FString& Path : SelectedPaths)
		{
			FString PackagePath = Path;
			const FString AllPrefix = TEXT("/All");
			if (PackagePath.StartsWith(AllPrefix))
			{
				PackagePath = PackagePath.RightChop(AllPrefix.Len());
			}
			Filter.PackagePaths.Add(FName(*PackagePath));
		}

		TArray<FAssetData> FolderAssets;
		AssetRegistry.GetAssets(Filter, FolderAssets);

		for (const FAssetData& FolderAsset : FolderAssets)
		{
			Assets.AddUnique(FolderAsset);
		}
	}
#endif

	// Nothing selected — do nothing rather than opening an empty audit.
	if (Assets.IsEmpty()) { return; }

	RequestLoadThenAudit(Assets);
}

void UELTEditorAuditor::AuditAssets(const TArray<FAssetData>& Assets)
{
	RequestLoadThenAudit(Assets);
}