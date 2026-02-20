// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#pragma once

#include "ELTEditorSettings.h"

ELTEDITOR_PRAGMA_DISABLE_OPTIMIZATION

#define ELTE_GET_SETTING(_SettingName) return GetDefault<UELTEditorSettings>()->_SettingName;

#if (ENGINE_MAJOR_VERSION == 5)
#define ELTE_SET_SETTING(_SettingName, _SettingValue) UELTEditorSettings* Settings = GetMutableDefault<UELTEditorSettings>(); \
Settings->_SettingName = _SettingValue; \
Settings->TryUpdateDefaultConfigFile();
#else
#define ELTE_SET_SETTING(_SettingName, _SettingValue) UELTEditorSettings* Settings = GetMutableDefault<UELTEditorSettings>(); \
Settings->_SettingName = _SettingValue; \
Settings->UpdateDefaultConfigFile();
#endif

FString UELTEditorSettings::GetLocalizationPath()
{
	ELTE_GET_SETTING(LocalizationPath);
}

void UELTEditorSettings::SetLocalizationPath(const FString& NewLocalizationPath)
{
	ELTE_SET_SETTING(LocalizationPath, NewLocalizationPath);
}


TMap<FString, FString> UELTEditorSettings::GetCSVPaths()
{
	ELTE_GET_SETTING(CSVPaths);
}

void UELTEditorSettings::SetCSVPaths(const TMap<FString, FString>& NewCSVPaths)
{
	ELTE_SET_SETTING(CSVPaths, NewCSVPaths);
}


bool UELTEditorSettings::GetLocalizationPreview()
{
	ELTE_GET_SETTING(bLocalizationPreview);
}

void UELTEditorSettings::SetLocalizationPreview(bool bNewPreview)
{
	ELTE_SET_SETTING(bLocalizationPreview, bNewPreview);
}


FString UELTEditorSettings::GetLocalizationPreviewLang()
{
	ELTE_GET_SETTING(LocalizationPreviewLang);
}

void UELTEditorSettings::SetLocalizationPreviewLang(const FString& NewLocalizationPreviewLang)
{
	ELTE_SET_SETTING(LocalizationPreviewLang, NewLocalizationPreviewLang);
}


bool UELTEditorSettings::GetReimportAtEditorStartup()
{
	ELTE_GET_SETTING(bReimportAtEditorStartup);
}

void UELTEditorSettings::SetReimportAtEditorStartup(bool bNewReimportAtEditorStartup)
{
	ELTE_SET_SETTING(bReimportAtEditorStartup, bNewReimportAtEditorStartup);
}

bool UELTEditorSettings::GetPreviewInUIEnabled()
{
	ELTE_GET_SETTING(bPreviewInUI);
}

void UELTEditorSettings::SetPreviewInUIEnabled(bool bNewPreviewInUIEnabled)
{
	ELTE_SET_SETTING(bPreviewInUI, bNewPreviewInUIEnabled);
}

TMap<FString, FString> UELTEditorSettings::GetGlobalNamespaces()
{
	ELTE_GET_SETTING(GlobalNamespaces);
}

void UELTEditorSettings::SetGlobalNamespace(const TMap<FString, FString>& NewGlobalNamespaces)
{
	ELTE_SET_SETTING(GlobalNamespaces, NewGlobalNamespaces);
}

FString UELTEditorSettings::GetSeparator()
{
	ELTE_GET_SETTING(Separator);
}

void UELTEditorSettings::SetSeparator(const FString& NewSeparator)
{
	ELTE_SET_SETTING(Separator, NewSeparator);
}

FString UELTEditorSettings::GetFallbackWhenEmpty()
{
	ELTE_GET_SETTING(FallbackWhenEmpty);
}

void UELTEditorSettings::SetFallbackWhenEmpty(const FString& NewFallbackWhenEmpty)
{
	ELTE_SET_SETTING(FallbackWhenEmpty, NewFallbackWhenEmpty);
}

ELTEDITOR_PRAGMA_ENABLE_OPTIMIZATION