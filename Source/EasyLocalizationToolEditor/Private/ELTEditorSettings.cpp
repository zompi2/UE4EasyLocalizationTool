// Copyright (c) 2023 Damian Nowakowski. All rights reserved.

#pragma once

#include "ELTEditorSettings.h"

ELTEDITOR_PRAGMA_DISABLE_OPTIMIZATION

#define ELTE_GET_SETTING(_SettingName) return GetDefault<UELTEditorSettings>()->_SettingName;
#define ELTE_SET_SETTING(_SettingName, _SettingValue) UELTEditorSettings* Settings = GetMutableDefault<UELTEditorSettings>(); \
Settings->_SettingName = _SettingValue; \
Settings->SaveConfig();


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

TMap<FString, FString> UELTEditorSettings::GetGlobalNamespaces()
{
	ELTE_GET_SETTING(GlobalNamespaces);
}

void UELTEditorSettings::SetGlobalNamespace(const TMap<FString, FString>& NewGlobalNamespaces)
{
	ELTE_SET_SETTING(GlobalNamespaces, NewGlobalNamespaces);
}

ELTEDITOR_PRAGMA_ENABLE_OPTIMIZATION