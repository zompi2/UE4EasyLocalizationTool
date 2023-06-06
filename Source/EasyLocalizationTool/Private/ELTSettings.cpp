// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#pragma once

#include "ELTSettings.h"

#define ELT_GET_SETTING(_SettingName) return GetDefault<UELTSettings>()->_SettingName;
#define ELT_SET_SETTING(_SettingName, _SettingValue) UELTSettings* Settings = GetMutableDefault<UELTSettings>(); \
Settings->_SettingName = _SettingValue; \
Settings->UpdateDefaultConfigFile();

bool UELTSettings::GetManuallySetLastUsedLanguage()
{
	ELT_GET_SETTING(bManuallySetLastUsedLanguage);
}

void UELTSettings::SetManuallySetLastUsedLanguage(bool bNewManuallySetLastUsageLanguage)
{
	ELT_SET_SETTING(bManuallySetLastUsedLanguage, bNewManuallySetLastUsageLanguage);
}

bool UELTSettings::GetOverrideLanguageAtFirstLaunch()
{
	ELT_GET_SETTING(bOverrideLanguageAtFirstLaunch);
}

void UELTSettings::SetOverrideLanguageAtFirstLaunch(bool bNewOverridLanguageAtFirstLaunch)
{
	ELT_SET_SETTING(bOverrideLanguageAtFirstLaunch, bNewOverridLanguageAtFirstLaunch);
}

FString UELTSettings::GetLanguageToOverrideAtFirstLaunch()
{
	ELT_GET_SETTING(LanguageToOverrideAtFirstLaunch);
}

void UELTSettings::SetLanguageToOverrideAtFirstLaunch(FString NewLanguageToOverrideArFirstLaunch)
{
	ELT_SET_SETTING(LanguageToOverrideAtFirstLaunch, NewLanguageToOverrideArFirstLaunch);
}

TArray<FString> UELTSettings::GetAvailableLanguages()
{
	ELT_GET_SETTING(AvailableLangs);
}

void UELTSettings::SetAvailableLanguages(const TArray<FString>& NewAvailableLanguages)
{
	ELT_SET_SETTING(AvailableLangs, NewAvailableLanguages);
}