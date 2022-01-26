// Copyright (c) 2022 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ELT.generated.h"

/**
 * A subsystem which handles localizations in game.
 */

// Event launched when text localizations has changed due to the change of culture
DECLARE_EVENT(UELT, FOnTextLocalizationChanged)

UCLASS()
class EASYLOCALIZATIONTOOL_API UELT : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:

	// Begin of USubsystem interface
	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;
	// End of USubsystem interface

public:

	/**
	 * Returns code for currently used language.
	 */
	static FString GetCurrentLanguage();

	/**
	 * Returns a list of codes of all available languages.
	 */
	static TArray<FString> GetAvailableLanguages();

	/**
	 * Checks if the language of given code can be set.
	 */
	static bool CanSetLanguage(const FString& Lang);

	/**
	 * Sets the language represented by a given code.
	 * Returns true if the language was set with success.
	 */
	static bool SetLanguage(const FString& Lang);

	/**
	 * Gets an event which runs when text localizations has changed.
	 * Use it to bind functions to it.
	 */
	static FOnTextLocalizationChanged& GetOnTextLocalizationChanged();

private:

	// Name of a save file.
	static FString ELTSaveName;

	// Code of currently used language.
	static FString ELTCurrentLanguage;

	// Static instance of the Text Localization Changed event.
	static FOnTextLocalizationChanged OnTextLocalizationChangedEvent;

	// Handle for a delegate from TextLocalizationManager.
	FDelegateHandle OnTextRevisionChangedEventHandle;

	/**
	 * Broadcast event about text localization change.
	 */
	void BroadcastOnTextLocalizationChanged();

	// Flag to prevent changing languages in callbacks.
	static bool LanguageChangeLock;
};
