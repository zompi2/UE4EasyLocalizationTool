// Copyright (c) 2023 Damian Nowakowski. All rights reserved.

#include "ELT.h"
#include "ELTSettings.h"
#include "ELTSave.h"
#include "Internationalization/Internationalization.h"
#include "Internationalization/Culture.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "UObject/Package.h"

ELT_PRAGMA_DISABLE_OPTIMIZATION

const FString ELTSaveName = TEXT("ELTSave");

UELT* UELT::Get(const UWorld* World)
{
	if (World)
	{
		if (UGameInstance* GI = World->GetGameInstance())
		{
			return GI->GetSubsystem<UELT>();
		}
	}
	return nullptr;
}

void UELT::Initialize(FSubsystemCollectionBase& Collection)
{
	if (HasAnyFlags(EObjectFlags::RF_ClassDefaultObject) == false)
	{
		// Setup all default values
		ELTCurrentLanguage = TEXT("");
		LanguageChangeLock = false;
		AllowToUseSaveFiles = false;

		// Ensure all culture data is loaded at this point.
		FInternationalization::Get().LoadAllCultureData();

		// Bind an event to the text localization change.
		OnTextRevisionChangedEventHandle = FTextLocalizationManager::Get().OnTextRevisionChangedEvent.AddUObject(this, &UELT::BroadcastOnTextLocalizationChanged);

		// Set lastly used language. If will get a lastly used language from a save file or a default one if manual set last language is on.
		SetLastUsedLanguage();
	}
}

void UELT::Deinitialize()
{
	if (HasAnyFlags(EObjectFlags::RF_ClassDefaultObject) == false)
	{
		// Cleanup the binding to the text localization change event.
		FTextLocalizationManager::Get().OnTextRevisionChangedEvent.Remove(OnTextRevisionChangedEventHandle);

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
		// Load lastly used language from a save file only when the settings are not set to do it manually.
		if (UELTSettings::GetManuallySetLastUsedLanguage() == false)
		{
			if (UGameplayStatics::DoesSaveGameExist(ELTSaveName, 0))
			{
				if (UELTSave* LoadedSave = Cast<UELTSave>(UGameplayStatics::LoadGameFromSlot(ELTSaveName, 0)))
				{
					ELTCurrentLanguage = LoadedSave->SavedCurrentLanguage;
				}
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
	// Allow to change language if there is no lock set up.
	if (LanguageChangeLock == false)
	{ 
		// Language can be set only if it is available in a list of available languages.
		if (Lang.IsEmpty() == false)
		{
			if (UELTSettings::GetAvailableLanguages().Contains(Lang))
			{
				return true;
			}
		}
	}
	
	return false;
}

bool UELT::SetLanguage(const FString& Lang)
{
	// Check if the language can be set
	if (CanSetLanguage(Lang))
	{
		// Set up change lock. It will prevent changing languages in on localization change events.
		LanguageChangeLock = true;

		// Because setting language might trigger on text localization change event set the current language
		// to a desired language right away. Save the old language in order to fall back if the setting language
		// fails.
		FString OldLanguage = ELTCurrentLanguage;
		ELTCurrentLanguage = Lang;

		// Always set the language, even if the desired language is the same as Current Language.
		// Current Language is received from save file, but it doesn't mean it has already been applied!
		if (FInternationalization::Get().SetCurrentLanguage(ELTCurrentLanguage))
		{
			// If the desired language is different than current old language - save it.
			// Do not save it if manual set last used language is enabled.
			if (UELTSettings::GetManuallySetLastUsedLanguage() == false)
			{
				if (OldLanguage != ELTCurrentLanguage)
				{
					UELTSave* Save = NewObject<UELTSave>(GetTransientPackage(), UELTSave::StaticClass());
					Save->SavedCurrentLanguage = ELTCurrentLanguage;
					UGameplayStatics::SaveGameToSlot(Save, ELTSaveName, 0);
				}
			}

			// Ensure all localization resources are properly loaded for this language.
			RefreshLanguageResources();

			// Release the change lock.
			LanguageChangeLock = false;
			return true;
		}
		else
		{
			// Localization change failed - set back to the previous language.
			ELTCurrentLanguage = OldLanguage;
		}

		// Release the change lock.
		LanguageChangeLock = false;
	}
	return false;
}

void UELT::RefreshLanguageResources()
{
	// Refresh resources.
	FTextLocalizationManager::Get().RefreshResources();

	// If this is an editor (also play in editor) - enable game localization preview, so the 
	// localization set/refresh will have a visible effect.
	// (Also, because RefreshResources disables localization preview)
#if WITH_EDITOR
	if (GIsEditor)
	{
		FTextLocalizationManager::Get().EnableGameLocalizationPreview(ELTCurrentLanguage);
	}
#endif
}

void UELT::SetLastUsedLanguage()
{
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
			// Otherwise remember the current local language.
			SetLanguage(FInternationalization::Get().GetCurrentLanguage()->GetName());
		}
	}
	else
	{
		// Current language was available in a save file - set it at startup.
		SetLanguage(LanguageToSet);
	}
}

void UELT::BroadcastOnTextLocalizationChanged()
{
	OnTextLocalizationChangedStatic.Broadcast();
	OnTextLocalizationChanged.Broadcast();
}

ELT_PRAGMA_ENABLE_OPTIMIZATION