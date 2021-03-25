// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#pragma once

#include "ELTEditorSettings.h"

#define ELTE_GET_SETTING(_SettingName) return GetDefault<UELTEditorSettings>()->_SettingName;
#define ELTE_SET_SETTING(_SettingName, _SettingValue) UELTEditorSettings* Settings = GetMutableDefault<UELTEditorSettings>(); \
Settings->_SettingName = _SettingValue; \
Settings->SaveConfig();


FString UELTEditorSettings::GetLocalisationPath()
{
	ELTE_GET_SETTING(LocalisationPath);
}

void UELTEditorSettings::SetLocalisationPath(const FString& NewLocalisationPath)
{
	ELTE_SET_SETTING(LocalisationPath, NewLocalisationPath);
}


TMap<FString, FString> UELTEditorSettings::GetCSVPaths()
{
	ELTE_GET_SETTING(CSVPaths);
}

void UELTEditorSettings::SetCSVPaths(const TMap<FString, FString>& NewCSVPaths)
{
	ELTE_SET_SETTING(CSVPaths, NewCSVPaths);
}


bool UELTEditorSettings::GetLocalisationPreview()
{
	ELTE_GET_SETTING(bLocalisationPreview);
}

void UELTEditorSettings::SetLocalisationPreview(bool bNewPreview)
{
	ELTE_SET_SETTING(bLocalisationPreview, bNewPreview);
}


FString UELTEditorSettings::GetLocalisationPreviewLang()
{
	ELTE_GET_SETTING(LocalisationPreviewLang);
}

void UELTEditorSettings::SetLocalisationPreiewLang(const FString& NewLocalisationPreviewLang)
{
	ELTE_SET_SETTING(LocalisationPreviewLang, NewLocalisationPreviewLang);
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