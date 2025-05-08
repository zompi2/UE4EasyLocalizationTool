// Copyright (c) 2025 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ELT.generated.h"

/**
 * A subsystem which handles localizations in game.
 */

// Event launched when text localizations has changed due to the change of culture.
// This is a dynamic version, that can be binded via BP and to UFUNCTIONs in C++.
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTextLocalizationChanged);

// Event launched when text localizations has changed due to the change of culture.
// This is a static version, that can be binded via C++ only.
DECLARE_EVENT(UELT, FOnTextLocalizationChangedStatic);

// A convenience macro for getting an ELT subsystem.
#define GetELT() UELT::Get(GetWorld())

UCLASS()
class EASYLOCALIZATIONTOOL_API UELT : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:

	// Begin of USubsystem interface.
	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;
	// End of USubsystem interface.

public:

	/**
	 * A static getter for this subsystem.
	 */
	static UELT* Get(const UWorld* World);

	/**
	 * Returns code for currently used language.
	 */
	UFUNCTION(BlueprintPure, Category = "Easy Localization Tool")
	FString GetCurrentLanguage();

	/**
	 * Returns a list of codes of all available languages.
	 */
	UFUNCTION(BlueprintPure, Category = "Easy Localization Tool")
	TArray<FString> GetAvailableLanguages();

	/**
	 * Checks if the language of given code can be set.
	 */
	UFUNCTION(BlueprintPure, Category = "Easy Localization Tool")
	bool CanSetLanguage(const FString& Lang);

	/**
	 * Sets the language represented by a given code.
	 * Returns true if the language was set with success.
	 */
	UFUNCTION(BlueprintCallable, Category = "Easy Localization Tool")
	bool SetLanguage(const FString& Lang);

	/**
	 * Refreshes current language resources.
	 * Might be needed when some texts are not displayed properly.
	 */
	UFUNCTION(BlueprintCallable, Category = "Easy Localization Tool")
	void RefreshLanguageResources();

	/**
	 * An event which runs when text localizations has changed (dynamic version).
	 */
	UPROPERTY(BlueprintAssignable)
	FOnTextLocalizationChanged OnTextLocalizationChanged;

	/**
	 * An event which runs when text localizations has changed (static version).
	 */
	FOnTextLocalizationChangedStatic OnTextLocalizationChangedStatic;

private:

	// Code of currently used language.
	FString ELTCurrentLanguage;

	// Handle for a delegate from TextLocalizationManager.
	FDelegateHandle OnTextRevisionChangedEventHandle;

	// Flag to prevent changing languages in callbacks.
	bool LanguageChangeLock;

	// Flag to prevent from getting and setting language from save file while it is not available yet.
	bool AllowToUseSaveFiles;

	/**
	 * Broadcast event about text localization change.
	 */
	void BroadcastOnTextLocalizationChanged();

	/**
	 * Internal implementation of LoadLastUsedLanguage.
	 */
	void SetLastUsedLanguage();

	/**
	 * Applies ELTCurrentLanguage to the localization. There will be different behaviour
	 * when playing in the editor and when playing standalone. In the editor only preview
	 * should be setup, otherwise the whole engine localization will change.
	 */
	bool ApplyCurrentLanguage();
};
