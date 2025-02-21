// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

#pragma once

#include "ELTSettings.h"
#include "Runtime/Launch/Resources/Version.h"

ELT_PRAGMA_DISABLE_OPTIMIZATION

#define ELT_GET_SETTING(_SettingName) return GetDefault<UELTSettings>()->_SettingName;

#if (ENGINE_MAJOR_VERSION == 5)
#define ELT_SET_SETTING(_SettingName, _SettingValue) UELTSettings* Settings = GetMutableDefault<UELTSettings>(); \
Settings->_SettingName = _SettingValue; \
Settings->TryUpdateDefaultConfigFile();
#else
#define ELT_SET_SETTING(_SettingName, _SettingValue) UELTSettings* Settings = GetMutableDefault<UELTSettings>(); \
Settings->_SettingName = _SettingValue; \
Settings->UpdateDefaultConfigFile();
#endif

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

bool UELTSettings::GetLogDebug()
{
	ELT_GET_SETTING(bLogDebug);
}

void UELTSettings::SetLogDebug(bool bNewLogDebug)
{
	ELT_SET_SETTING(bLogDebug, bNewLogDebug);
}

ELT_PRAGMA_ENABLE_OPTIMIZATION