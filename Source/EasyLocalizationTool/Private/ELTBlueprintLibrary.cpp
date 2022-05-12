// Copyright (c) 2022 Damian Nowakowski. All rights reserved.

#include "ELTBlueprintLibrary.h"
#include "ELT.h"

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
		FTextInspector::GetNamespace(InText).GetValue().Split(TEXT(" "), &OutNamespace, &OutPackage);
		OutKey = FTextInspector::GetKey(InText).GetValue();
		if (const FString* Source = FTextInspector::GetSourceString(InText))
			OutSource = *Source;
		else
			OutSource = TEXT("");
	}
}
