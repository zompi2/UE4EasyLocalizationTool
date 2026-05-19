// Copyright (c) 2026 Crezetique. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ELTEditorAuditTypes.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"

class STableViewBase;
class ITableRow;
class SDockTab;

class SELTEditorAuditWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SELTEditorAuditWidget) {}
		SLATE_ARGUMENT(TArray<FELTAssetAuditResult>, AuditResults)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	void Refresh(const TArray<FELTAssetAuditResult>& InResults);
	void RefreshLanguages(const TArray<FString>& Languages);
	void SetPendingCompletionDialog(bool bPending) { bPendingCompletionDialog = bPending; }

	static void JumpToIssue(const FAssetData& AssetData, const FELTAuditIssue& Issue);

private:
	using FIssuePtr = TSharedPtr<FELTAuditIssue>;
	using FLanguagePtr = TSharedPtr<FString>;

	// Full unfiltered list
	TArray<FIssuePtr> AllIssues;
	
	// Filtered + Sorted view
	TArray<FIssuePtr> FlatIssues;
	
	// Key AssetName, OnGenerateIssueRow.
	TMap<FString, FAssetData> AssetDataMap;

	// All asset data from the last audit run, used to reaudit via Refresh().
	TArray<FAssetData> LastAuditedAssets;
	
	// Widget Pointers
	TSharedPtr<STextBlock> SummaryText;
	TSharedPtr<STextBlock> TipText;
	TSharedPtr<SHeaderRow> AuditHeaderRow;
	TSharedPtr<SListView<FIssuePtr>> ListView;

	// Language Dropdown
	TArray<FLanguagePtr> AvailableLanguages;
	TSharedPtr<SComboBox<FLanguagePtr>> LanguageComboBox;
	FLanguagePtr SelectedLanguage;

	// Hide rows with no issue
	bool bFilterIssues = false;
	
	// Hide rows with empty text values
	bool bHideEmpty = false;
	
	// Shows more details in list
	bool bShowMoreDetails = true;

	// When true, Refresh() will show the audit completion dialog after updating the widget.
	bool bPendingCompletionDialog = false;

	// More Detail columns
	static const TArray<FName>& GetDetailColumns()
	{
		static const TArray<FName> Cols = {
			TEXT("LocalizedString"),
			TEXT("Value"),
			TEXT("Key"),
			TEXT("Namespace"),
			TEXT("IsUsingStringTable")
		};
		return Cols;
	}
	
	FName SortColumn;
	EColumnSortMode::Type SortMode = EColumnSortMode::None;
	
	void  RebuildAuditData(const TArray<FELTAssetAuditResult>& InResults);
	FText BuildSummaryText() const;
	FText PickRandomTip() const;
	
	TSharedRef<ITableRow> OnGenerateIssueRow(FIssuePtr Issue, const TSharedRef<STableViewBase>& OwnerTable);
	void OnSortColumn(EColumnSortPriority::Type Priority, const FName& Column, EColumnSortMode::Type Mode);
	void OnFilterIssuesChanged(ECheckBoxState NewState);
	void OnHideEmptyChanged(ECheckBoxState NewState);
	void OnShowMoreDetailsChanged(ECheckBoxState NewState);
	void OnLanguageSelected(FLanguagePtr Item, ESelectInfo::Type SelectInfo);
	TSharedRef<SWidget> OnGenerateLanguageComboRow(FLanguagePtr Item);
	FReply OnRefreshAuditClicked();
	FReply OnReimportCSVClicked();

	void ApplySort();
	void ApplyFilter();
	void ApplyDetailColumnVisibility();
	EColumnSortMode::Type GetSortModeForColumn(FName Column) const;
};

DECLARE_DELEGATE(FOnAuditWidgetReimportCSV);

class EASYLOCALIZATIONTOOLEDITOR_API UELTEditorAuditWidget
{
public:
	static void ShowResults(const TArray<FELTAssetAuditResult>& Results);

	// Called after a CSV reimport to refresh the language dropdown with newly available languages.
	static void UpdateAvailableLanguages(const TArray<FString>& Languages);

	// Shows the audit complete dialog. Called from Refresh() once the widget is live.
	static void ShowCompletionDialog(const TArray<FELTAssetAuditResult>& Results);

	static const FName TabName;

	// Bound by UELTEditor to trigger CSV reimport when the Reimport CSV button is clicked.
	static FOnAuditWidgetReimportCSV OnReimportCSVDelegate;

	// Set before SpawnTab is called so Construct can read it synchronously.
	static bool bPendingDialogOnSpawn;

private:
	static TSharedRef<SDockTab> SpawnTab(const FSpawnTabArgs& Args);

	static TArray<FELTAssetAuditResult>    PendingResults;
	static TWeakPtr<SELTEditorAuditWidget> LiveWidget;
};