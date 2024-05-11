// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

#include "ELTEditorStyle.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"

ELTEDITOR_PRAGMA_DISABLE_OPTIMIZATION

// Null declaration of static variable (for linker needs)
TSharedPtr<FSlateStyleSet> FELTEditorStyle::StyleInstance = nullptr;

void FELTEditorStyle::Initialize()
{
	if (StyleInstance.IsValid() == false)
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FELTEditorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

void FELTEditorStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

FName FELTEditorStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("ELTEditorStyle"));
	return StyleSetName;
}

TSharedRef<FSlateStyleSet> FELTEditorStyle::Create()
{
	// Create a new Style Set with a content root set to Resources directory of the plugin.
	TSharedRef<FSlateStyleSet> Style = MakeShareable(new FSlateStyleSet(GetStyleSetName()));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("EasyLocalizationTool")->GetBaseDir() / TEXT("Resources"));

	// Create a new Slate Image Brush, which is Icon16.png from Resources directory.
	FSlateImageBrush* Brush = new FSlateImageBrush(Style->RootToContentDir(TEXT("Icon16"), TEXT(".png")), { 16.f, 16.f });
	
	// Add newly created Brush to the Style Set.
	Style->Set("ELTEditorStyle.MenuIcon", Brush);
	
	// Result is a Style Set with menu icon in it.
	return Style;
}

ELTEDITOR_PRAGMA_ENABLE_OPTIMIZATION