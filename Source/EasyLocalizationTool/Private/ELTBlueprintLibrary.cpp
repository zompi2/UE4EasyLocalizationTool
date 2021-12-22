// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

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
