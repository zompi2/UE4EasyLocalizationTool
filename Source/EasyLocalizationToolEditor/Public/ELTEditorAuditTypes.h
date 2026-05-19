// Copyright (c) 2026 Crezetique. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ELTEditorAuditTypes.generated.h"

UENUM(BlueprintType)
enum class EELTAuditParameterType : uint8
{
	// CDO-level variable or Blueprint class variable.
	ClassVariable		UMETA(DisplayName = "Class Variable"),

	// Local variable or input/output parameter on a Blueprint function graph.
	FunctionVariable	UMETA(DisplayName = "Function Variable"),

	// Inline literal FText pin on a Blueprint graph node.
	NodeParameter		UMETA(DisplayName = "Node Parameter"),

	// FText property on a UMG widget component inside a Widget Blueprint's widget tree.
	WidgetComponent		UMETA(DisplayName = "Widget Component"),
};

UENUM(BlueprintType)
enum class EELTAuditIssueType : uint8
{
	// FText passed the audit — no localization issue detected.
	None					UMETA(DisplayName = "-"),

	// FText field is empty.
	EmptyValue				UMETA(DisplayName = "Empty Value"),

	// String Table reference is broken — table missing from registry or key absent from it.
	StringTableMissingKey	UMETA(DisplayName = "String Table Missing Key"),

	// Key does not match the source value — text was never set up via ELT or was edited after.
	NotYetLocalized			UMETA(DisplayName = "Not Yet Localized"),

	// Key matches source but is absent from the live localization table.
	InvalidLocalization		UMETA(DisplayName = "Invalid Localization"),
};

// One row in the audit results table.
USTRUCT(BlueprintType)
struct FELTAuditIssue
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "ELT Audit")
	FString AssetName;

	UPROPERTY(BlueprintReadOnly, Category = "ELT Audit")
	EELTAuditParameterType Type = EELTAuditParameterType::ClassVariable;

	// Variable name, pin name, or node display name depending on Type.
	UPROPERTY(BlueprintReadOnly, Category = "ELT Audit")
	FString VariableNodeName;

	UPROPERTY(BlueprintReadOnly, Category = "ELT Audit")
	EELTAuditIssueType Issue = EELTAuditIssueType::None;

	UPROPERTY(BlueprintReadOnly, Category = "ELT Audit")
	FString Value;

	// The resolved localization string for the current language at the time of audit.
	UPROPERTY(BlueprintReadOnly, Category = "ELT Audit")
	FString LocalizedString;

	UPROPERTY(BlueprintReadOnly, Category = "ELT Audit")
	FString Key;

	UPROPERTY(BlueprintReadOnly, Category = "ELT Audit")
	FString Namespace;

	// True when the FText is backed by a String Table reference.
	UPROPERTY(BlueprintReadOnly, Category = "ELT Audit")
	bool bIsUsingStringTable = false;

	// Navigation data used by the Quick Action column — not displayed as table columns.
	UPROPERTY(BlueprintReadOnly, Category = "ELT Audit")
	FGuid NodeGuid;

	UPROPERTY(BlueprintReadOnly, Category = "ELT Audit")
	FString GraphName;

	bool HasIssue() const { return Issue != EELTAuditIssueType::None; }
	bool IsEmpty()  const { return Issue == EELTAuditIssueType::EmptyValue; }

	FString TypeDisplayString() const
	{
		switch (Type)
		{
		case EELTAuditParameterType::ClassVariable:    return TEXT("Class Variable");
		case EELTAuditParameterType::FunctionVariable: return TEXT("Function Variable");
		case EELTAuditParameterType::NodeParameter:    return TEXT("Node Parameter");
		case EELTAuditParameterType::WidgetComponent:  return TEXT("Widget Component");
		}
		return TEXT("");
	}

	FString IssueDisplayString() const
	{
		switch (Issue)
		{
		case EELTAuditIssueType::None:                  return TEXT("-");
		case EELTAuditIssueType::EmptyValue:            return TEXT("Empty Value");
		case EELTAuditIssueType::StringTableMissingKey: return TEXT("String Table Missing Key");
		case EELTAuditIssueType::NotYetLocalized:       return TEXT("Not Yet Localized");
		case EELTAuditIssueType::InvalidLocalization:   return TEXT("Invalid Localization");
		}
		return TEXT("");
	}

	FString IssueTooltipString() const
	{
		switch (Issue)
		{
		case EELTAuditIssueType::None:                  return TEXT("");
		case EELTAuditIssueType::EmptyValue:            return TEXT("This FText field has no value set. \n\nIf this is intentional for state or function processing purposes, it is recommended to toggle FText localize bool to false.");
		case EELTAuditIssueType::StringTableMissingKey: return TEXT("String Table is set, but an invalid key is selected.");
		case EELTAuditIssueType::NotYetLocalized:       return TEXT("Unable to fetch a localization string. (Value and Key do not match)");
		case EELTAuditIssueType::InvalidLocalization:   return TEXT("The localization key does not return a valid localization string.");
		}
		return TEXT("");
	}

	FString ToLogString() const
	{
		return FString::Printf(TEXT("[%s] [%s] %s  Value='%s'  NS='%s'  Key='%s'"),
			*TypeDisplayString(), *IssueDisplayString(), *VariableNodeName, *Value, *Namespace, *Key);
	}
};

// Aggregates all issues found for a single asset. The widget flattens these into one list.
USTRUCT(BlueprintType)
struct FELTAssetAuditResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "ELT Audit")
	FAssetData AssetData;

	UPROPERTY(BlueprintReadOnly, Category = "ELT Audit")
	TArray<FELTAuditIssue> Issues;

	bool HasIssues() const
	{
		return Issues.ContainsByPredicate([](const FELTAuditIssue& I){ return I.HasIssue(); });
	}
};