// Copyright (c) 2022 Damian Nowakowski. All rights reserved.

#include "ELTBlueprintLibrary.h"
#include "ELT.h"

FString UELTBlueprintLibrary::GetCurrentLanguage()
{
	return UELT::GetCurrentLanguage();
}

TArray<FString> UELTBlueprintLibrary::GetAvailableLanguages()
{
	return UELT::GetAvailableLanguages();
}

bool UELTBlueprintLibrary::CanSetLanguage(const FString& Language)
{
	return UELT::CanSetLanguage(Language);
}

bool UELTBlueprintLibrary::SetLanguage(const FString& Language)
{
	return UELT::SetLanguage(Language);
}

FText UELTBlueprintLibrary::Conv_LocTextToText(FLocText InLocText)
{
	return InLocText.GetText();
}

FString UELTBlueprintLibrary::Conv_LocTextToString(FLocText InLocText)
{
	return InLocText.ToString();
}

void UELTBlueprintLibrary::GetTextData(FText InText, FString& OutPackage, FString& OutNamespace, FString& OutKey)
{
	FTextInspector::GetNamespace(InText).GetValue().Split(TEXT(" "), &OutNamespace, &OutPackage);
	OutKey = FTextInspector::GetKey(InText).GetValue();
}