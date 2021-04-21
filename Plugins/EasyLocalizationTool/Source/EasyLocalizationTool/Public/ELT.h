// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ELT.generated.h"

/**
 * A subsystem which handles localizations in game.
 */

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
	 * Checks if a language of given code can be set.
	 */
	static bool CanSetLanguage(const FString& Lang);

	/**
	 * Sets a language represented by a given code.
	 * Returns true if language was set with success.
	 */
	static bool SetLanguage(const FString& Lang);

private:

	// Name of a save file.
	static FString ELTSaveName;

	// Code of currently used language.
	static FString CurrentLanguage;
};
