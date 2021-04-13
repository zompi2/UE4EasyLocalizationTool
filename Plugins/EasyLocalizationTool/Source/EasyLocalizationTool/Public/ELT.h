// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ELT.generated.h"

UCLASS()
class EASYLOCALIZATIONTOOL_API UELT : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:

	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;

public:

	static FString GetCurrentLanguage();

	static bool CanSetLanguage(const FString& Lang);
	static bool SetLanguage(const FString& Lang);

	static TArray<FString> GetAvailableLanguages();

private:

	static FString ELTSaveName;
};
