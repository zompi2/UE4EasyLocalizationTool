// Copyright (c) 2023 Damian Nowakowski. All rights reserved.

#include "ELTBlueprintLibrary.h"
#include "ELT.h"

ELT_PRAGMA_DISABLE_OPTIMIZATION

FString UELTBlueprintLibrary::GetCurrentLanguage(const UObject* WorldContextObject)
{
	if (UWorld* ThisWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull))
		if (UELT* ELT = UELT::Get(ThisWorld))
			return ELT->GetCurrentLanguage();
	return TEXT("");
}

TArray<FString> UELTBlueprintLibrary::GetAvailableLanguages(const UObject* WorldContextObject)
{
	if (UWorld* ThisWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull))
		if (UELT* ELT = UELT::Get(ThisWorld))
			return ELT->GetAvailableLanguages();
	return {};
}

bool UELTBlueprintLibrary::CanSetLanguage(const UObject* WorldContextObject, const FString& Language)
{
	if (UWorld* ThisWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull))
		if (UELT* ELT = UELT::Get(ThisWorld))
			return ELT->CanSetLanguage(Language);
	return false;
}

bool UELTBlueprintLibrary::SetLanguage(const UObject* WorldContextObject, const FString& Language)
{
	if (UWorld* ThisWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull))
		if (UELT* ELT = UELT::Get(ThisWorld))
			return ELT->SetLanguage(Language);
	return false;
}

void UELTBlueprintLibrary::RefreshLanguageResources(const UObject* WorldContextObject)
{
	if (UWorld* ThisWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull))
		if (UELT* ELT = UELT::Get(ThisWorld))
			ELT->RefreshLanguageResources();
}

FText UELTBlueprintLibrary::Conv_LocTextToText(FLocText InLocText)
{
	return InLocText.GetText();
}

FString UELTBlueprintLibrary::Conv_LocTextToString(FLocText InLocText)
{
	return InLocText.ToString();
}

void UELTBlueprintLibrary::GetTextData(FText InText, FString& OutPackage, FString& OutNamespace, FString& OutKey, FString& OutSource)
{
	if (InText.IsEmpty() == false)
	{
		TOptional<FString> Namespace = FTextInspector::GetNamespace(InText);
		if (Namespace.IsSet())
		{
			Namespace.GetValue().Split(TEXT(" "), &OutNamespace, &OutPackage);
		}

		TOptional<FString> Key = FTextInspector::GetKey(InText);
		if (Key.IsSet())
		{
			OutKey = Key.GetValue();
		}

		if (const FString* Source = FTextInspector::GetSourceString(InText))
		{
			OutSource = *Source;
		}
		else
		{
			OutSource = TEXT("");
		}
	}
}

bool UELTBlueprintLibrary::AreTextKeysEqual(const FText& A, const FText& B)
{
	TOptional<FString> KeyA = FTextInspector::GetKey(A);
	TOptional<FString> KeyB = FTextInspector::GetKey(B);

	if (KeyA.IsSet() && KeyB.IsSet())
	{
		return (KeyA.GetValue() == KeyB.GetValue());
	}

	return false;
}

ELT_PRAGMA_ENABLE_OPTIMIZATION