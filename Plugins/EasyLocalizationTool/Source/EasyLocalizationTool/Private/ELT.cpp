// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#include "ELT.h"
#include "Internationalization/Internationalization.h"
#include "Internationalization/Culture.h"
#include "ELTSettings.h"
#include "Kismet/GameplayStatics.h"
#include "ELTSave.h"

FString UELT::ELTSaveName = TEXT("ELTSave");

void UELT::Initialize(FSubsystemCollectionBase& Collection)
{
	if (HasAnyFlags(EObjectFlags::RF_ClassDefaultObject) == false)
	{
		FInternationalization::Get().LoadAllCultureData();
		const FString& CurrentLanguage = GetCurrentLanguage();
		if (CurrentLanguage.IsEmpty())
		{
			if (UELTSettings::GetOverrideLanguageAtFirstLaunch() && CanSetLanguage(UELTSettings::GetLanguageToOverrideAtFirstLaunch()))
			{
				SetLanguage(UELTSettings::GetLanguageToOverrideAtFirstLaunch());
			}
			else
			{
				SetLanguage(FInternationalization::Get().GetCurrentLanguage()->GetName());
			}
		}
		else
		{
			SetLanguage(CurrentLanguage);
		}
	}
}

void UELT::Deinitialize()
{
	if (HasAnyFlags(EObjectFlags::RF_ClassDefaultObject) == false)
	{
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
	if (UGameplayStatics::DoesSaveGameExist(ELTSaveName, 0))
	{
		if (UELTSave* LoadedSave = Cast<UELTSave>(UGameplayStatics::LoadGameFromSlot(ELTSaveName, 0)))
		{
			return LoadedSave->CurrentLanguage;
		}
	}
	return TEXT("");
}

bool UELT::CanSetLanguage(const FString& Lang)
{
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
	if (CanSetLanguage(Lang))
	{
		UELTSave* Save = NewObject<UELTSave>(GetTransientPackage(), UELTSave::StaticClass());
		Save->CurrentLanguage = Lang;
		UGameplayStatics::SaveGameToSlot(Save, ELTSaveName, 0);

		FInternationalization::Get().SetCurrentLanguage(Lang);

#if WITH_EDITOR
		if (GIsEditor)
		{
			FTextLocalizationManager::Get().EnableGameLocalizationPreview(Lang);
		}
#endif
		return true;
	}
	return false;
}

TArray<FString> UELT::GetAvailableLanguages()
{
	return UELTSettings::GetAvailableLanguages();
}

