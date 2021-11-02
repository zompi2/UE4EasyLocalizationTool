// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#include "ELT.h"
#include "ELTSettings.h"
#include "ELTSave.h"
#include "Internationalization/Internationalization.h"
#include "Internationalization/Culture.h"
#include "Kismet/GameplayStatics.h"

// Definition of static save file name value.
FString UELT::ELTSaveName = TEXT("ELTSave");

// Definition of static current language value.
FString UELT::ELTCurrentLanguage = TEXT("");

void UELT::Initialize(FSubsystemCollectionBase& Collection)
{
	if (HasAnyFlags(EObjectFlags::RF_ClassDefaultObject) == false)
	{
		// Ensure all culture data is loaded at this point
		FInternationalization::Get().LoadAllCultureData();

		const FString& LanguageToSet = GetCurrentLanguage();
		if (LanguageToSet.IsEmpty())
		{
			if (UELTSettings::GetOverrideLanguageAtFirstLaunch() && CanSetLanguage(UELTSettings::GetLanguageToOverrideAtFirstLaunch()))
			{
				// If no current language is set in save file and the "override language at first launch" is set to an available language - set this language.
				SetLanguage(UELTSettings::GetLanguageToOverrideAtFirstLaunch());
			}
			else
			{
				// Otherwise remember the current local language
				SetLanguage(FInternationalization::Get().GetCurrentLanguage()->GetName());
			}
		}
		else
		{
			// Current language was available in a save file - set it at startup.
			SetLanguage(LanguageToSet);
		}
	}
}

void UELT::Deinitialize()
{
	if (HasAnyFlags(EObjectFlags::RF_ClassDefaultObject) == false)
	{
		// For editor - ensure the localization preview is disabled.
#if WITH_EDITOR
		if (GIsEditor)
		{
			FTextLocalizationManager::Get().DisableGameLocalizationPreview();
		}
#endif
	}
}

FString UELT::GetCurrentLanguage()
{
	// If Current Language is not cached - try to load it from a save file.
	if (ELTCurrentLanguage.IsEmpty())
	{
		if (UGameplayStatics::DoesSaveGameExist(ELTSaveName, 0))
		{
			if (UELTSave* LoadedSave = Cast<UELTSave>(UGameplayStatics::LoadGameFromSlot(ELTSaveName, 0)))
			{
				ELTCurrentLanguage = LoadedSave->SavedCurrentLanguage;
			}
		}
	}
	return ELTCurrentLanguage;
}

TArray<FString> UELT::GetAvailableLanguages()
{
	// Simply return a list from settings (from .ini file).
	return UELTSettings::GetAvailableLanguages();
}

bool UELT::CanSetLanguage(const FString& Lang)
{
	// Language can be set only if it is available in a list of available languages.
	if (Lang.IsEmpty() == false)
	{
		if (UELTSettings::GetAvailableLanguages().Contains(Lang))
		{
			return true;
		}
	}
	return false;
}

bool UELT::SetLanguage(const FString& Lang)
{
	// Check if the language can be set
	if (CanSetLanguage(Lang))
	{
		// Always set the language, even if the desired language is the same as Current Language.
		// Current Language is received from save file, but it doesn't mean it has already been applied!
		if (FInternationalization::Get().SetCurrentLanguage(Lang))
		{
			// If the desired language is different than current language - save it.
			// Do not save if they are the same to avoid frequent writes to disk.
			if (ELTCurrentLanguage != Lang)
			{
				ELTCurrentLanguage = Lang;

				UELTSave* Save = NewObject<UELTSave>(GetTransientPackage(), UELTSave::StaticClass());
				Save->SavedCurrentLanguage = Lang;
				UGameplayStatics::SaveGameToSlot(Save, ELTSaveName, 0);
			}
			
			// If this is an editor (also play in editor) - enable game localization preview, so the 
			// localization set will have a visible effect.
#if WITH_EDITOR
			if (GIsEditor)
			{
				FTextLocalizationManager::Get().EnableGameLocalizationPreview(Lang);
			}
#endif
			return true;
		}
	}
	return false;
}
