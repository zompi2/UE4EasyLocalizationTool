// Copyright (c) 2026 Crezetique. All rights reserved.

#include "ELTEditorAuditWidget.h"
#include "ELTEditorAuditor.h"
#include "ELTSettings.h"

#include "Framework/Docking/TabManager.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Input/SHyperlink.h"
#include "Widgets/Views/SHeaderRow.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/SBoxPanel.h"

#include "Engine/Blueprint.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Internationalization/Internationalization.h"
#include "Internationalization/Culture.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Misc/MessageDialog.h"
#include "Editor.h"

#if (ENGINE_MAJOR_VERSION >= 5)
	#define ELT_APP_STYLE FAppStyle::Get()
	#define ELT_GET_BRUSH(Name) FAppStyle::GetBrush(Name)
#else
	#include "EditorStyleSet.h"
	#define ELT_APP_STYLE FEditorStyle::Get()
	#define ELT_GET_BRUSH(Name) FEditorStyle::GetBrush(Name)
#endif

DEFINE_LOG_CATEGORY_STATIC(ELTAuditWidgetLog, Log, All);

namespace ELTAuditCol
{
	static const FName AssetName          = TEXT("AssetName");
	static const FName Type               = TEXT("Type");
	static const FName VariableNode       = TEXT("VariableNode");
	static const FName Issue              = TEXT("Issue");
	static const FName QuickAction        = TEXT("QuickAction");
	static const FName LocalizedString    = TEXT("LocalizedString");
	static const FName Value              = TEXT("Value");
	static const FName Key                = TEXT("Key");
	static const FName Namespace          = TEXT("Namespace");
	static const FName IsUsingStringTable = TEXT("IsUsingStringTable");
}

namespace ELTAuditColor
{
	// Cell text colours
	static const FLinearColor Default         = FLinearColor::White;
	static const FLinearColor Muted           = FLinearColor(0.5f, 0.5f, 0.5f);

	// Type column
	static const FLinearColor TypeClassVariable    = FLinearColor(.7f,  0.4f, 0.9f);
	static const FLinearColor TypeFunctionVariable = FLinearColor(0.4f, 0.7f, 1.f);
	static const FLinearColor TypeNodeParameter    = FLinearColor(1.f, 0.8f, 0.2f);
	static const FLinearColor TypeWidgetComponent  = FLinearColor(0.4f, 0.9f, 0.4f);

	// Issue column
	static const FLinearColor IssueNone    = FLinearColor(0.5f, 0.5f, 0.5f);
	static const FLinearColor IssueWarning = FLinearColor(1.f, 0.8f, 0.2f);
	static const FLinearColor IssueError   = FLinearColor(1.f, 0.4f, 0.4f);

	// IsUsingStringTable column
	static const FLinearColor StringTableTrue  = FLinearColor(0.4f, 0.9f, 0.4f);
	static const FLinearColor StringTableFalse = FLinearColor(1.f,  0.4f, 0.4f);

	// Primary column background tint
	static const FLinearColor PrimaryColumnTint = FLinearColor(1.f, 1.f, 1.f, 0.05f);

	// Tip text
	static const FLinearColor TipText = FLinearColor::White;
}

const FName UELTEditorAuditWidget::TabName = TEXT("ELTLocalizationAudit");
TArray<FELTAssetAuditResult> UELTEditorAuditWidget::PendingResults;
TWeakPtr<SELTEditorAuditWidget> UELTEditorAuditWidget::LiveWidget;
FOnAuditWidgetReimportCSV UELTEditorAuditWidget::OnReimportCSVDelegate;
bool UELTEditorAuditWidget::bPendingDialogOnSpawn = false;

/*static*/ void UELTEditorAuditWidget::ShowCompletionDialog(const TArray<FELTAssetAuditResult>& Results)
{
	int32 IssueCount = 0;
	TSet<FString> AffectedAssets;
	for (const FELTAssetAuditResult& Result : Results)
	{
		for (const FELTAuditIssue& Issue : Result.Issues)
		{
			if (Issue.HasIssue())
			{
				++IssueCount;
				AffectedAssets.Add(Issue.AssetName);
			}
		}
	}

	const FString Summary = (IssueCount == 0)
		? FString::Printf(TEXT("All %d asset(s) passed localization audit."), Results.Num())
		: FString::Printf(TEXT("%d issue(s) found across %d / %d asset(s)."), IssueCount, AffectedAssets.Num(), Results.Num());

#if (ENGINE_MAJOR_VERSION == 5)
	const EAppMsgCategory Category = (IssueCount == 0) ? EAppMsgCategory::Success : EAppMsgCategory::Warning;
	FMessageDialog::Open(Category, EAppMsgType::Ok, FText::FromString(Summary), FText::FromString(TEXT("Audit Complete")));
#else
	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(Summary));
#endif
}

/*static*/ void UELTEditorAuditWidget::ShowResults(const TArray<FELTAssetAuditResult>& Results)
{
	PendingResults = Results;

	FGlobalTabmanager& TM = *FGlobalTabmanager::Get();

	if (TM.HasTabSpawner(TabName) == false)
	{
		FTabSpawnerEntry& Entry = TM.RegisterNomadTabSpawner(
			TabName, FOnSpawnTab::CreateStatic(&UELTEditorAuditWidget::SpawnTab));

		Entry
			.SetDisplayName(FText::FromString(TEXT("Localization Audit")))
			.SetTooltipText(FText::FromString(TEXT("Shows FText localization issues found in selected assets.")))
			.SetMenuType(ETabSpawnerMenuType::Hidden);
	}

	if (TSharedPtr<SELTEditorAuditWidget> Pinned = LiveWidget.Pin())
	{
		Pinned->SetPendingCompletionDialog(true);
		Pinned->Refresh(Results);
		TM.TryInvokeTab(TabName);
		return;
	}

	// Mark that a completion dialog should show when the widget is constructed.
	bPendingDialogOnSpawn = true;
	TM.TryInvokeTab(TabName);
}

/*static*/ void UELTEditorAuditWidget::UpdateAvailableLanguages(const TArray<FString>& Languages)
{
	if (TSharedPtr<SELTEditorAuditWidget> Pinned = LiveWidget.Pin())
	{
		Pinned->RefreshLanguages(Languages);
	}
}

/*static*/ TSharedRef<SDockTab> UELTEditorAuditWidget::SpawnTab(const FSpawnTabArgs& Args)
{
	TSharedPtr<SELTEditorAuditWidget> Widget;

	TSharedRef<SDockTab> Tab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		.Label(FText::FromString(TEXT("Localization Audit")))
		[
			SAssignNew(Widget, SELTEditorAuditWidget)
				.AuditResults(PendingResults)
		];

	LiveWidget = Widget;
	return Tab;
}

class SELTAuditIssueRow : public SMultiColumnTableRow<TSharedPtr<FELTAuditIssue>>
{
public:
	SLATE_BEGIN_ARGS(SELTAuditIssueRow) {}
		SLATE_ARGUMENT(TSharedPtr<FELTAuditIssue>, Issue)
		SLATE_ARGUMENT(FAssetData,AssetData)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTable)
	{
		Issue     = InArgs._Issue;
		AssetData = InArgs._AssetData;

		// Initialised here so it has the same lifetime as the row widget — safe to take &.
		CopyButtonStyle = FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("NoBorder");
		CopyButtonStyle.Normal  = FSlateNoResource();
		CopyButtonStyle.Hovered = FSlateNoResource();
		CopyButtonStyle.Pressed = FSlateNoResource();

		SMultiColumnTableRow<TSharedPtr<FELTAuditIssue>>::Construct(
			SMultiColumnTableRow::FArguments().Padding(FMargin(2.f, 1.f)), OwnerTable);
	}

	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnId) override
	{
		if (Issue.IsValid() == false)
		{
			return SNullWidget::NullWidget;
		}

		const FELTAuditIssue& Row = *Issue;

		// Detail columns have no background tint; primary columns (first five) get a subtle lighter shade.
		auto Cell = [this](const FString& Str, FLinearColor Col = ELTAuditColor::Default) -> TSharedRef<SWidget>
		{
			return SNew(SButton)
				.ButtonStyle(&CopyButtonStyle)
				.ContentPadding(FMargin(6.f, 2.f))
				.Cursor(EMouseCursor::Hand)
				.ToolTipText(FText::FromString(TEXT("Left-click to copy")))
				.OnClicked_Lambda([Str]() -> FReply
				{
					FPlatformApplicationMisc::ClipboardCopy(*Str);
					return FReply::Handled();
				})
				[
					SNew(STextBlock)
						.Text(FText::FromString(Str))
						.ColorAndOpacity(Col)
				];
		};

		auto PrimaryCell = [this](const FString& Str, FLinearColor Col = ELTAuditColor::Default) -> TSharedRef<SWidget>
		{
			return SNew(SBorder)
				.BorderImage(ELT_GET_BRUSH("WhiteTexture"))
				.BorderBackgroundColor(ELTAuditColor::PrimaryColumnTint)
				.Padding(0.f)
				[
					SNew(SButton)
						.ButtonStyle(&CopyButtonStyle)
						.ContentPadding(FMargin(6.f, 2.f))
						.Cursor(EMouseCursor::Hand)
						.ToolTipText(FText::FromString(TEXT("Left-click to copy")))
						.OnClicked_Lambda([Str]() -> FReply
						{
							FPlatformApplicationMisc::ClipboardCopy(*Str);
							return FReply::Handled();
						})
						[
							SNew(STextBlock)
								.Text(FText::FromString(Str))
								.ColorAndOpacity(Col)
						]
				];
		};

		if (ColumnId == ELTAuditCol::AssetName)
		{
			return PrimaryCell(Row.AssetName, ELTAuditColor::Default);
		}

		if (ColumnId == ELTAuditCol::QuickAction)
		{
			const FAssetData     CapturedAsset = AssetData;
			const FELTAuditIssue CapturedRow   = Row;

			FString ActionLabel;
			FString ActionTooltip;

			switch (Row.Type)
			{
			case EELTAuditParameterType::ClassVariable:
				ActionLabel   = TEXT("Open Asset");
				ActionTooltip = TEXT("Open this asset in its default editor");
				break;
			case EELTAuditParameterType::FunctionVariable:
				ActionLabel   = TEXT("Jump to Function");
				ActionTooltip = FString::Printf(TEXT("Navigate to function '%s'"), *Row.GraphName);
				break;
			case EELTAuditParameterType::NodeParameter:
				ActionLabel   = TEXT("Jump to Node");
				ActionTooltip = FString::Printf(TEXT("Focus the node in graph '%s'"), *Row.GraphName);
				break;
			case EELTAuditParameterType::WidgetComponent:
				ActionLabel   = TEXT("Open Asset");
				ActionTooltip = TEXT("Open this Widget Blueprint in the designer");
				break;
			}

			return SNew(SBorder)
				.BorderImage(ELT_GET_BRUSH("WhiteTexture"))
				.BorderBackgroundColor(ELTAuditColor::PrimaryColumnTint)
				.Padding(FMargin(6.f, 1.f))
				[
					SNew(SHyperlink)
						.Text(FText::FromString(ActionLabel))
						.ToolTipText(FText::FromString(ActionTooltip))
						.OnNavigate_Lambda([CapturedAsset, CapturedRow]()
						{
							SELTEditorAuditWidget::JumpToIssue(CapturedAsset, CapturedRow);
						})
				];
		}

		if (ColumnId == ELTAuditCol::Type)
		{
			FLinearColor TypeCol = ELTAuditColor::TypeClassVariable;
			if      (Row.Type == EELTAuditParameterType::FunctionVariable) { TypeCol = ELTAuditColor::TypeFunctionVariable; }
			else if (Row.Type == EELTAuditParameterType::NodeParameter)    { TypeCol = ELTAuditColor::TypeNodeParameter; }
			else if (Row.Type == EELTAuditParameterType::WidgetComponent)    { TypeCol = ELTAuditColor::TypeWidgetComponent; }
			return PrimaryCell(Row.TypeDisplayString(), TypeCol);
		}

		if (ColumnId == ELTAuditCol::VariableNode) { return PrimaryCell(Row.VariableNodeName, ELTAuditColor::Default); }

		if (ColumnId == ELTAuditCol::Issue)
		{
			FLinearColor IssueCol;
			if      (Row.Issue == EELTAuditIssueType::None)       { IssueCol = ELTAuditColor::IssueNone; }
			else if (Row.Issue == EELTAuditIssueType::EmptyValue) { IssueCol = ELTAuditColor::IssueWarning; }
			else                                                  { IssueCol = ELTAuditColor::IssueError; }

			return SNew(SBorder)
				.BorderImage(ELT_GET_BRUSH("WhiteTexture"))
				.BorderBackgroundColor(ELTAuditColor::PrimaryColumnTint)
				.Padding(FMargin(6.f, 2.f))
				[
					SNew(STextBlock)
						.Text(FText::FromString(Row.IssueDisplayString()))
						.ColorAndOpacity(IssueCol)
						.ToolTipText(FText::FromString(Row.IssueTooltipString()))
				];
		}

		if (ColumnId == ELTAuditCol::LocalizedString)    { return Cell(Row.LocalizedString,     ELTAuditColor::Muted); }
		if (ColumnId == ELTAuditCol::Value)              { return Cell(Row.Value,               ELTAuditColor::Muted); }
		if (ColumnId == ELTAuditCol::Key)                { return Cell(Row.Key,                 ELTAuditColor::Muted); }
		if (ColumnId == ELTAuditCol::Namespace)          { return Cell(Row.Namespace,           ELTAuditColor::Muted); }
		
		if (ColumnId == ELTAuditCol::IsUsingStringTable)
		{
			const bool bUsing = Row.bIsUsingStringTable;
			return Cell(bUsing ? TEXT("true") : TEXT("false"),
				bUsing ? ELTAuditColor::StringTableTrue : ELTAuditColor::StringTableFalse);
		}

		return SNullWidget::NullWidget;
	}

private:
	TSharedPtr<FELTAuditIssue> Issue;
	FAssetData AssetData;
	FButtonStyle CopyButtonStyle;
};

void SELTEditorAuditWidget::RebuildAuditData(const TArray<FELTAssetAuditResult>& InResults)
{
	AllIssues.Reset();
	FlatIssues.Reset();
	AssetDataMap.Reset();
	LastAuditedAssets.Reset();

	for (const FELTAssetAuditResult& Result : InResults)
	{
		AssetDataMap.Emplace(Result.AssetData.AssetName.ToString(), Result.AssetData);
		LastAuditedAssets.Add(Result.AssetData);

		for (const FELTAuditIssue& Issue : Result.Issues)
		{
			AllIssues.Add(MakeShared<FELTAuditIssue>(Issue));
		}
	}
}

FText SELTEditorAuditWidget::BuildSummaryText() const
{
	const int32 TotalAssets = AssetDataMap.Num();

	int32 IssueCount = 0;
	TSet<FString> AffectedAssets;
	for (const FIssuePtr& Ptr : AllIssues)
	{
		if (Ptr.IsValid() && Ptr->HasIssue())
		{
			++IssueCount;
			AffectedAssets.Add(Ptr->AssetName);
		}
	}

	if (IssueCount == 0)
	{
		return FText::FromString(FString::Printf(TEXT("✔  All %d asset(s) passed localization audit."), TotalAssets));
	}

	return FText::FromString(FString::Printf(TEXT("⚠  %d issue(s) across %d / %d asset(s)."), IssueCount, AffectedAssets.Num(), TotalAssets));
}

EColumnSortMode::Type SELTEditorAuditWidget::GetSortModeForColumn(FName Column) const
{
	return (SortColumn == Column) ? SortMode : EColumnSortMode::None;
}

void SELTEditorAuditWidget::OnSortColumn(EColumnSortPriority::Type /*Priority*/, const FName& Column, EColumnSortMode::Type Mode)
{
	SortColumn = Column;
	SortMode   = Mode;
	ApplySort();

	if (ListView.IsValid()) { ListView->RequestListRefresh(); }
}

void SELTEditorAuditWidget::ApplySort()
{
	if (SortMode == EColumnSortMode::None || SortColumn.IsNone())
	{
		return;
	}

	auto GetSortKey = [](const FIssuePtr& Ptr, const FName& Col) -> FString
	{
		if (Ptr.IsValid() == false) { return TEXT(""); }
		const FELTAuditIssue& I = *Ptr;
		if (Col == ELTAuditCol::AssetName)          { return I.AssetName; }
		if (Col == ELTAuditCol::Type)               { return I.TypeDisplayString(); }
		if (Col == ELTAuditCol::VariableNode)       { return I.VariableNodeName; }
		if (Col == ELTAuditCol::Issue)              { return I.IssueDisplayString(); }
		if (Col == ELTAuditCol::LocalizedString)    { return I.LocalizedString; }
		if (Col == ELTAuditCol::Value)              { return I.Value; }
		if (Col == ELTAuditCol::Key)                { return I.Key; }
		if (Col == ELTAuditCol::Namespace)          { return I.Namespace; }
		if (Col == ELTAuditCol::IsUsingStringTable) { return I.bIsUsingStringTable ? TEXT("true") : TEXT("false"); }
		return TEXT("");
	};

	const FName Col     = SortColumn;
	const bool  bAscend = (SortMode == EColumnSortMode::Ascending);

	FlatIssues.Sort([&](const FIssuePtr& A, const FIssuePtr& B)
	{
		const int32 Cmp = GetSortKey(A, Col).Compare(GetSortKey(B, Col), ESearchCase::IgnoreCase);
		return bAscend ? Cmp < 0 : Cmp > 0;
	});
}

void SELTEditorAuditWidget::ApplyFilter()
{
	FlatIssues.Reset();
	for (const FIssuePtr& Ptr : AllIssues)
	{
		if (Ptr.IsValid() == false) { continue; }

		if (Ptr->Issue == EELTAuditIssueType::None)
		{
			if (bFilterIssues == false) { FlatIssues.Add(Ptr); }
		}
		else if (Ptr->Issue == EELTAuditIssueType::EmptyValue)
		{
			if (bHideEmpty == false) { FlatIssues.Add(Ptr); }
		}
		else
		{
			FlatIssues.Add(Ptr);
		}
	}
}

void SELTEditorAuditWidget::OnFilterIssuesChanged(ECheckBoxState NewState)
{
	bFilterIssues = (NewState == ECheckBoxState::Checked);
	ApplyFilter();
	ApplySort();
	if (ListView.IsValid())    { ListView->RequestListRefresh(); }
	if (SummaryText.IsValid()) { SummaryText->SetText(BuildSummaryText()); }
}

void SELTEditorAuditWidget::OnHideEmptyChanged(ECheckBoxState NewState)
{
	bHideEmpty = (NewState == ECheckBoxState::Checked);
	ApplyFilter();
	ApplySort();
	if (ListView.IsValid())    { ListView->RequestListRefresh(); }
	if (SummaryText.IsValid()) { SummaryText->SetText(BuildSummaryText()); }
}

void SELTEditorAuditWidget::OnLanguageSelected(FLanguagePtr Item, ESelectInfo::Type SelectInfo)
{
	if (Item.IsValid() == false) { return; }

	SelectedLanguage = Item;
	FInternationalization::Get().SetCurrentLanguage(*Item);

	// Reaudit the same assets so LocalizedString values reflect the new language.
	const TArray<FELTAssetAuditResult> Results = UELTEditorAuditor::RunAudit(LastAuditedAssets);
	Refresh(Results);
}

TSharedRef<SWidget> SELTEditorAuditWidget::OnGenerateLanguageComboRow(FLanguagePtr Item)
{
	return SNew(STextBlock)
		.Text(FText::FromString(Item.IsValid() ? *Item : TEXT("")))
		.Margin(FMargin(4.f, 2.f));
}

void SELTEditorAuditWidget::ApplyDetailColumnVisibility()
{
	if (AuditHeaderRow.IsValid() == false) { return; }

#if (ENGINE_MAJOR_VERSION >= 5)
	for (const FName& Col : GetDetailColumns())
	{
		AuditHeaderRow->SetShowGeneratedColumn(Col, bShowMoreDetails);
	}
#endif
	// SetShowGeneratedColumn is not available in UE4 — detail columns are always visible there.
}

void SELTEditorAuditWidget::OnShowMoreDetailsChanged(ECheckBoxState NewState)
{
	bShowMoreDetails = (NewState == ECheckBoxState::Checked);
	ApplyDetailColumnVisibility();
}

FReply SELTEditorAuditWidget::OnRefreshAuditClicked()
{
	const TArray<FELTAssetAuditResult> Results = UELTEditorAuditor::RunAudit(LastAuditedAssets);
	bPendingCompletionDialog = true;
	Refresh(Results);
	return FReply::Handled();
}

FReply SELTEditorAuditWidget::OnReimportCSVClicked()
{
	// GenerateLocFilesImpl calls LoadPackage synchronously. If async loading is already in process, subsequent loads trigger an assertion.
	if (IsAsyncLoading())
	{
		UE_LOG(ELTAuditWidgetLog, Warning, TEXT("Reimport CSV skipped — async loading is in progress. Please wait a moment and try again."));
		return FReply::Handled();
	}

	UELTEditorAuditWidget::OnReimportCSVDelegate.ExecuteIfBound();
	return FReply::Handled();
}

FText SELTEditorAuditWidget::PickRandomTip() const
{
	static const TArray<FString> Tips = {
		TEXT("Copy to Clipboard by Left-Clicking audit table cells. Useful for copying Values to input on a separate localization sheet."),
		TEXT("Sort the audit table content by clicking the column headers."),
	};

	const int32 Index = FMath::RandRange(0, Tips.Num() - 1);
	return FText::FromString(FString::Printf(TEXT("Tip: %s"), *Tips[Index]));
}

void SELTEditorAuditWidget::Construct(const FArguments& InArgs)
{
	RebuildAuditData(InArgs._AuditResults);

	const TArray<FString> Langs = UELTSettings::GetAvailableLanguages();
	for (const FString& Lang : Langs)
	{
		AvailableLanguages.Add(MakeShared<FString>(Lang));
	}

	// Default current language, fallback to the first available.
	const FString CurrentLang = FInternationalization::Get().GetCurrentLanguage()->GetName();
	for (const FLanguagePtr& LangPtr : AvailableLanguages)
	{
		if (LangPtr.IsValid() && *LangPtr == CurrentLang)
		{
			SelectedLanguage = LangPtr;
			break;
		}
	}
	if (SelectedLanguage.IsValid() == false && AvailableLanguages.Num() > 0)
	{
		SelectedLanguage = AvailableLanguages[0];
	}

	// Set sort state before building the header so the attribute delegate returns
	SortColumn = ELTAuditCol::AssetName;
	SortMode   = EColumnSortMode::Ascending;

	SAssignNew(AuditHeaderRow, SHeaderRow)

		+ SHeaderRow::Column(ELTAuditCol::AssetName)
			.DefaultLabel(FText::FromString(TEXT("Asset Name")))
			.FillWidth(0.11f)
			.SortMode(this, &SELTEditorAuditWidget::GetSortModeForColumn, ELTAuditCol::AssetName)
			.OnSort(this, &SELTEditorAuditWidget::OnSortColumn)

		+ SHeaderRow::Column(ELTAuditCol::Type)
			.DefaultLabel(FText::FromString(TEXT("Type")))
			.FillWidth(0.10f)
			.SortMode(this, &SELTEditorAuditWidget::GetSortModeForColumn, ELTAuditCol::Type)
			.OnSort(this, &SELTEditorAuditWidget::OnSortColumn)

		+ SHeaderRow::Column(ELTAuditCol::VariableNode)
			.DefaultLabel(FText::FromString(TEXT("Variable/Node Name")))
			.FillWidth(0.13f)
			.SortMode(this, &SELTEditorAuditWidget::GetSortModeForColumn, ELTAuditCol::VariableNode)
			.OnSort(this, &SELTEditorAuditWidget::OnSortColumn)

		+ SHeaderRow::Column(ELTAuditCol::Issue)
			.DefaultLabel(FText::FromString(TEXT("Issue")))
			.FillWidth(0.12f)
			.SortMode(this, &SELTEditorAuditWidget::GetSortModeForColumn, ELTAuditCol::Issue)
			.OnSort(this, &SELTEditorAuditWidget::OnSortColumn)

		+ SHeaderRow::Column(ELTAuditCol::QuickAction)
			.DefaultLabel(FText::FromString(TEXT("Quick Action")))
			.FixedWidth(110.f)

		+ SHeaderRow::Column(ELTAuditCol::LocalizedString)
			.DefaultLabel(FText::FromString(TEXT("Localized String")))
			.FillWidth(0.13f)
			.SortMode(this, &SELTEditorAuditWidget::GetSortModeForColumn, ELTAuditCol::LocalizedString)
			.OnSort(this, &SELTEditorAuditWidget::OnSortColumn)

		+ SHeaderRow::Column(ELTAuditCol::Value)
			.DefaultLabel(FText::FromString(TEXT("Value")))
			.FillWidth(0.13f)
			.SortMode(this, &SELTEditorAuditWidget::GetSortModeForColumn, ELTAuditCol::Value)
			.OnSort(this, &SELTEditorAuditWidget::OnSortColumn)

		+ SHeaderRow::Column(ELTAuditCol::Key)
			.DefaultLabel(FText::FromString(TEXT("Key")))
			.FillWidth(0.09f)
			.SortMode(this, &SELTEditorAuditWidget::GetSortModeForColumn, ELTAuditCol::Key)
			.OnSort(this, &SELTEditorAuditWidget::OnSortColumn)

		+ SHeaderRow::Column(ELTAuditCol::Namespace)
			.DefaultLabel(FText::FromString(TEXT("Namespace")))
			.FillWidth(0.10f)
			.SortMode(this, &SELTEditorAuditWidget::GetSortModeForColumn, ELTAuditCol::Namespace)
			.OnSort(this, &SELTEditorAuditWidget::OnSortColumn)

		+ SHeaderRow::Column(ELTAuditCol::IsUsingStringTable)
			.DefaultLabel(FText::FromString(TEXT("Is Using String Table")))
			.FillWidth(0.09f)
			.SortMode(this, &SELTEditorAuditWidget::GetSortModeForColumn, ELTAuditCol::IsUsingStringTable)
			.OnSort(this, &SELTEditorAuditWidget::OnSortColumn);

	SAssignNew(ListView, SListView<FIssuePtr>)
		.ListItemsSource(&FlatIssues)
		.OnGenerateRow(this, &SELTEditorAuditWidget::OnGenerateIssueRow)
		.HeaderRow(AuditHeaderRow.ToSharedRef())
		.SelectionMode(ESelectionMode::Single);

	ApplyFilter();
	ApplySort();
	ApplyDetailColumnVisibility();

	ChildSlot
	[
		SNew(SVerticalBox)

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(8.f, 8.f, 8.f, 4.f)
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.FillWidth(1.f)
			[
				SNew(STextBlock)
					.Text(FText::FromString(TEXT("Localization Audit")))
					.TextStyle(ELT_APP_STYLE, "LargeText")
			]

			+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.AutoWidth()
				.Padding(0.f, 0.f, 12.f, 0.f)
			[
				SNew(SCheckBox)
					.IsChecked(ECheckBoxState::Unchecked)
					.OnCheckStateChanged(this, &SELTEditorAuditWidget::OnFilterIssuesChanged)
					.ToolTipText(FText::FromString(TEXT("When checked, FText entries that passed the audit are hidden")))
				[
					SNew(STextBlock).Text(FText::FromString(TEXT("Hide Valid Localization")))
				]
			]

			+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.AutoWidth()
				.Padding(0.f, 0.f, 12.f, 0.f)
			[
				SNew(SCheckBox)
					.IsChecked(ECheckBoxState::Unchecked)
					.OnCheckStateChanged(this, &SELTEditorAuditWidget::OnHideEmptyChanged)
					.ToolTipText(FText::FromString(TEXT("When checked, FText fields with no value set are hidden")))
				[
					SNew(STextBlock).Text(FText::FromString(TEXT("Hide Empty")))
				]
			]

			+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.AutoWidth()
				.Padding(0.f, 0.f, 12.f, 0.f)
			[
				SNew(SCheckBox)
					.IsChecked(ECheckBoxState::Checked)
					.OnCheckStateChanged(this, &SELTEditorAuditWidget::OnShowMoreDetailsChanged)
					.ToolTipText(FText::FromString(TEXT("When checked, additional detail columns are shown (Localized String, Value, Key, Namespace, Is Using String Table)")))
				[
					SNew(STextBlock).Text(FText::FromString(TEXT("Show More Details")))
				]
			]

			+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.AutoWidth()
				.Padding(0.f, 0.f, 8.f, 0.f)
			[
				SAssignNew(LanguageComboBox, SComboBox<FLanguagePtr>)
					.OptionsSource(&AvailableLanguages)
					.InitiallySelectedItem(SelectedLanguage)
					.OnSelectionChanged(this, &SELTEditorAuditWidget::OnLanguageSelected)
					.OnGenerateWidget(this, &SELTEditorAuditWidget::OnGenerateLanguageComboRow)
					.ToolTipText(FText::FromString(TEXT("Preview language for Localized String column")))
				[
					SNew(STextBlock)
						.Text_Lambda([this]()
						{
							return FText::FromString(SelectedLanguage.IsValid() ? *SelectedLanguage : TEXT(""));
						})
				]
			]

			+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.AutoWidth()
				.Padding(0.f, 0.f, 8.f, 0.f)
			[
				SNew(SButton)
					.Text(FText::FromString(TEXT("Reaudit Assets")))
					.ToolTipText(FText::FromString(TEXT("Re-run the audit on all previously audited assets")))
					.OnClicked(this, &SELTEditorAuditWidget::OnRefreshAuditClicked)
			]

			+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.AutoWidth()
			[
				SNew(SButton)
					.Text(FText::FromString(TEXT("Reimport CSV")))
					.ToolTipText(FText::FromString(TEXT("Reimport CSVs defined within the Easy Localization Tool.")))
					.OnClicked(this, &SELTEditorAuditWidget::OnReimportCSVClicked)
			]
		]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(8.f, 0.f, 8.f, 6.f)
		[
			SNew(SBorder)
				.BorderImage(ELT_GET_BRUSH("ToolPanel.GroupBorder"))
				.Padding(FMargin(8.f, 4.f))
			[
				SAssignNew(SummaryText, STextBlock)
					.Text(BuildSummaryText())
					.TextStyle(ELT_APP_STYLE, "NormalText.Important")
			]
		]

			+ SVerticalBox::Slot()
			.FillHeight(1.f)
			.Padding(8.f, 0.f, 8.f, 4.f)
		[
			SNew(SBorder)
				.BorderImage(ELT_GET_BRUSH("ToolPanel.GroupBorder"))
			[
				ListView.ToSharedRef()
			]
		]

		+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(8.f, 0.f, 8.f, 8.f)
		[
			SAssignNew(TipText, STextBlock)
				.Text(PickRandomTip())
				.ColorAndOpacity(ELTAuditColor::TipText)
		]
	];

	// Fire the completion dialog now that the widget is fully constructed.
	if (UELTEditorAuditWidget::bPendingDialogOnSpawn)
	{
		UELTEditorAuditWidget::bPendingDialogOnSpawn = false;
		UELTEditorAuditWidget::ShowCompletionDialog(InArgs._AuditResults);
	}
}

void SELTEditorAuditWidget::Refresh(const TArray<FELTAssetAuditResult>& InResults)
{
	RebuildAuditData(InResults);
	ApplyFilter();
	ApplySort();

	if (ListView.IsValid())    { ListView->RequestListRefresh(); }
	if (SummaryText.IsValid()) { SummaryText->SetText(BuildSummaryText()); }
	if (TipText.IsValid())     { TipText->SetText(PickRandomTip()); }

	if (bPendingCompletionDialog)
	{
		bPendingCompletionDialog = false;
		UELTEditorAuditWidget::ShowCompletionDialog(InResults);
	}
}

void SELTEditorAuditWidget::RefreshLanguages(const TArray<FString>& Languages)
{
	AvailableLanguages.Reset();
	for (const FString& Lang : Languages)
	{
		AvailableLanguages.Add(MakeShared<FString>(Lang));
	}

	// Preserve the current selection if it still exists in the new list, otherwise fall back to the first entry.
	bool bSelectionStillValid = false;
	if (SelectedLanguage.IsValid())
	{
		for (const FLanguagePtr& LangPtr : AvailableLanguages)
		{
			if (LangPtr.IsValid() && *LangPtr == *SelectedLanguage)
			{
				SelectedLanguage = LangPtr;
				bSelectionStillValid = true;
				break;
			}
		}
	}

	if (bSelectionStillValid == false && AvailableLanguages.Num() > 0)
	{
		SelectedLanguage = AvailableLanguages[0];
	}

	if (LanguageComboBox.IsValid())
	{
		LanguageComboBox->RefreshOptions();
		LanguageComboBox->SetSelectedItem(SelectedLanguage);
	}
}

TSharedRef<ITableRow> SELTEditorAuditWidget::OnGenerateIssueRow(
	FIssuePtr                         Issue,
	const TSharedRef<STableViewBase>& OwnerTable)
{
	FAssetData RowAsset;
	if (Issue.IsValid())
	{
		if (const FAssetData* Found = AssetDataMap.Find(Issue->AssetName))
		{
			RowAsset = *Found;
		}
	}

	return SNew(SELTAuditIssueRow, OwnerTable)
		.Issue(Issue)
		.AssetData(RowAsset);
}

/*static*/ void SELTEditorAuditWidget::JumpToIssue(
	const FAssetData&     AssetData,
	const FELTAuditIssue& Issue)
{
	UObject* Asset = AssetData.GetAsset();
	if (Asset == nullptr) { return; }

	UAssetEditorSubsystem* AssetEditorSS = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();

	if (Issue.Type == EELTAuditParameterType::ClassVariable ||
		Issue.Type == EELTAuditParameterType::WidgetComponent)
	{
		AssetEditorSS->OpenEditorForAsset(Asset);
		return;
	}

	UBlueprint* Blueprint = Cast<UBlueprint>(Asset);
	if (Blueprint == nullptr)
	{
		AssetEditorSS->OpenEditorForAsset(Asset);
		return;
	}

	if (Issue.Type == EELTAuditParameterType::FunctionVariable && Issue.GraphName.IsEmpty() == false)
	{
		for (UEdGraph* Graph : Blueprint->FunctionGraphs)
		{
			if (Graph != nullptr && Graph->GetName() == Issue.GraphName)
			{
				FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(Graph);
				return;
			}
		}
		FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(Blueprint);
		return;
	}

	if (Issue.Type == EELTAuditParameterType::NodeParameter && Issue.NodeGuid.IsValid())
	{
		TArray<UEdGraph*> AllGraphs;
		Blueprint->GetAllGraphs(AllGraphs);

		for (UEdGraph* Graph : AllGraphs)
		{
			if (Graph == nullptr) { continue; }
			for (UEdGraphNode* Node : Graph->Nodes)
			{
				if (Node != nullptr && Node->NodeGuid == Issue.NodeGuid)
				{
					FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(Node);
					return;
				}
			}
		}

		FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(Blueprint);
	}
}