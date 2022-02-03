// Copyright (c) 2022 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ELT.generated.h"

/**
 * A subsystem which handles localizations in game.
 */

// Event launched when text localizations has changed due to the change of culture.
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTextLocalizationChanged);

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
	 * An event which runs when text localizations has changed.
	 */
	UPROPERTY(BlueprintAssignable)
	FOnTextLocalizationChanged OnTextLocalizationChanged;

private:

	// Code of currently used language.
	FString ELTCurrentLanguage;

	// Handle for a delegate from TextLocalizationManager.
	FDelegateHandle OnTextRevisionChangedEventHandle;

	// Flag to prevent changing languages in callbacks.
	bool LanguageChangeLock;

	/**
	 * Broadcast event about text localization change.
	 */
	void BroadcastOnTextLocalizationChanged();
};
