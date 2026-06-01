// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#include "Misc/AutomationTest.h"
#include "ELTEditor.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Internationalization/TextLocalizationResource.h"
#include "Internationalization/StringTableRegistry.h"
#include "Internationalization/StringTableCore.h"
#include "ELTEditorSettings.h"
#include "ELT.h"
#include "ELTEditorUtils.h"
#include "ELTBlueprintLibrary.h"

ELTTESTS_PRAGMA_DISABLE_OPTIMIZATION

class FELTAutomationCommon
{
public:

	static FString GetTestCSVDir()
	{
		return FPaths::ProjectPluginsDir() / TEXT("EasyLocalizationTool/Source/EasyLocalizationToolTests/TestData");
	}

	static FString GetTestCSVPath(const FString& TestName)
	{
		return GetTestCSVDir() / TestName + TEXT(".csv");
	}

	static void MakeTestDirectory()
	{
		IFileManager::Get().MakeDirectory(*GetTestCSVDir(), true);
	}

	static void CleanTestDirectory()
	{
		IFileManager::Get().DeleteDirectory(*GetTestCSVDir(), true);
	}

	static bool WriteTestCSV(const FString& Filename, const FString& Content)
	{
		MakeTestDirectory();
		const FString FilePath = GetTestCSVPath(Filename);
		return FFileHelper::SaveStringToFile(Content, *FilePath);
	}

	static bool LocResFileExists(const FString& LocPath, const FString& LocNameOld, const FString& Language)
	{
		FString FilePath = LocPath / Language / LocNameOld + TEXT(".locres");
		return IFileManager::Get().FileExists(*FilePath);
	}

	static bool StringTableFileExists(const FString& LocPath, const FString& LocNameOld, const FString& Namespace)
	{
		FString FilePath = LocPath / UELTEditor::GetStringTableName(LocNameOld, Namespace) + TEXT(".uasset");
		return IFileManager::Get().FileExists(*FilePath);
	}
};

#define TEST_TRANSLATION(_Lang, _Namespace, _Key, _Expected) { \
FTextLocalizationManager::Get().DisableGameLocalizationPreview(); \
FTextLocalizationManager::Get().EnableGameLocalizationPreview(TEXT(_Lang)); \
FString Result = NSLOCTEXT(_Namespace, _Key, _Key).ToString(); \
TestTrue(FString::Printf(TEXT("Test lang "##_Lang" of namespace: "##_Namespace" key: "##_Key" expect: "##_Expected" result: %s"), *Result), Result.Equals(##_Expected)); \
}

#define TEST_STRING_TABLE(_Lang, _Namespace, _Key, _Expected) { \
const FString StringTableName = UELTEditor::GetStringTableName(LocName, _Namespace); \
const FString StringTableID = FPackageName::FilenameToLongPackageName(LocPath / StringTableName + TEXT(".uasset")) + TEXT(".") + StringTableName; \
FTextLocalizationManager::Get().DisableGameLocalizationPreview(); \
FTextLocalizationManager::Get().EnableGameLocalizationPreview(_Lang); \
FText LocalizedText = FText::FromStringTable(FName(StringTableID), TEXT(_Key), EStringTableLoadingPolicy::FindOrLoad); \
FString Result = LocalizedText.ToString(); \
TestTrue(FString::Printf(TEXT("Test String Table: %s lang "##_Lang" key: "##_Key" expect: "##_Expected" result: %s"), *StringTableID, *Result), Result.Equals(##_Expected)); \
}

#define TEST_LOCFILE_EXISTS(_Lang) { \
TestTrue(TEXT(#_Lang" exists"), FELTAutomationCommon::LocResFileExists(LocPath, LocName, _Lang)); \
}

#define TEST_STRING_TABLE_EXISTS(_Namespace) { \
TestTrue(TEXT(#_Namespace" string table exists"), FELTAutomationCommon::StringTableFileExists(LocPath, LocName, TEXT(_Namespace))); \
}

BEGIN_DEFINE_SPEC(FELTTests, "EasyLocalizationTool.Tests", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
FString LocName = TEXT("Game");
FString LocPath = UELTEditorSettings::GetLocalizationPath();
FString CSVName = TEXT("CSVTest");
END_DEFINE_SPEC(FELTTests)

void FELTTests::Define()
{
	Describe(TEXT("ELT - Import Tests"), [this]()
	{
		BeforeEach([]()
		{
			FELTAutomationCommon::MakeTestDirectory();
		});

		// ====================== 1. TESTS: VALID CSV WITH NAMESPACE ======================
		It(TEXT("VALID CSV WITH NAMESPACE"), [this]()
		{
			const FString CSV = TEXT("namespace,key,lang-en,lang-pl,lang-fr,lang-de\n")
				TEXT("MainMenu,StartGame,Start Game,Rozpocznij gre,Commencer le jeu,Spiel starten\n")
				TEXT("MainMenu,QuitGame,Quit,Wyjd ,Quitter,Beenden\n")
				TEXT("MainMenu,Settings,Settings,Ustawienia,Paramtres,Einstellungen\n")
				TEXT("MainMenu,Credits,Credits,Napisy,Cr dits,Abspann\n")
				TEXT("Gameplay,Health,Health,Zdrowie,Sant,Gesundheit\n")
				TEXT("Gameplay,Damage,Damage,Obra enia,D g ts,Schaden\n")
				TEXT("Gameplay,Score,Score,Wynik,Score,Punktzahl\n")
				TEXT("Gameplay,Level,Level,Poziom,Niveau,Stufe\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(CSVName, CSV));

			FString OutMessage;
			const bool bSuccess = UELTEditor::GenerateLocFilesImpl(
				FELTAutomationCommon::GetTestCSVPath(CSVName), 
				LocPath,
				LocName, 
				TEXT("GlobalNamespace"), // Global namespace
				TEXT(","), // Separator
				TEXT("NONE"), // Empty value fallback
				false, // Generate String Tables
				OutMessage); // Out Message

			TestTrue(TEXT("GenerateLocFilesImpl succeeded"), bSuccess);

			TEST_LOCFILE_EXISTS("en");
			TEST_LOCFILE_EXISTS("pl");
			TEST_LOCFILE_EXISTS("fr");
			TEST_LOCFILE_EXISTS("de");

			TEST_TRANSLATION("en", "MainMenu", "StartGame", "Start Game");
			TEST_TRANSLATION("pl", "MainMenu", "StartGame", "Rozpocznij gre");
			TEST_TRANSLATION("fr", "MainMenu", "StartGame", "Commencer le jeu");
			TEST_TRANSLATION("de", "MainMenu", "StartGame", "Spiel starten");

			TEST_TRANSLATION("en", "Gameplay", "Health", "Health");
			TEST_TRANSLATION("pl", "Gameplay", "Health", "Zdrowie");
			TEST_TRANSLATION("fr", "Gameplay", "Health", "Sant");
			TEST_TRANSLATION("de", "Gameplay", "Health", "Gesundheit");
		});

		// ====================== 2. TESTS: VALID CSV WITHOUT NAMESPACE ======================
		It(TEXT("VALID CSV WITHOUT NAMESPACE"), [this]()
		{
			const FString CSV = TEXT("key,lang-en,lang-pl,lang-fr,lang-de\n")
				TEXT("StartGame,Start Game,Rozpocznij gre,Commencer le jeu,Spiel starten\n")
				TEXT("QuitGame,Quit,Wyjd ,Quitter,Beenden\n")
				TEXT("Settings,Settings,Ustawienia,Paramtres,Einstellungen\n")
				TEXT("Credits,Credits,Napisy,Cr dits,Abspann\n")
				TEXT("Health,Health,Zdrowie,Sant,Gesundheit\n")
				TEXT("Damage,Damage,Obra enia,D g ts,Schaden\n")
				TEXT("Score,Score,Wynik,Score,Punktzahl\n")
				TEXT("Level,Level,Poziom,Niveau,Stufe\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(CSVName, CSV));

			FString OutMessage;
			const bool bSuccess = UELTEditor::GenerateLocFilesImpl(
				FELTAutomationCommon::GetTestCSVPath(CSVName), 
				LocPath,
				LocName, 
				TEXT("GlobalNamespace"), // Global namespace
				TEXT(","), // Separator
				TEXT("NONE"), // Empty value fallback
				false, // Generate String Tables
				OutMessage); // Out Message

			TestTrue(TEXT("GenerateLocFilesImpl succeeded"), bSuccess);

			TEST_LOCFILE_EXISTS("en");
			TEST_LOCFILE_EXISTS("pl");
			TEST_LOCFILE_EXISTS("fr");
			TEST_LOCFILE_EXISTS("de");

			TEST_TRANSLATION("en", "GlobalNamespace", "StartGame", "Start Game");
			TEST_TRANSLATION("pl", "GlobalNamespace", "StartGame", "Rozpocznij gre");
			TEST_TRANSLATION("fr", "GlobalNamespace", "StartGame", "Commencer le jeu");
			TEST_TRANSLATION("de", "GlobalNamespace", "StartGame", "Spiel starten");

			TEST_TRANSLATION("en", "GlobalNamespace", "Health", "Health");
			TEST_TRANSLATION("pl", "GlobalNamespace", "Health", "Zdrowie");
			TEST_TRANSLATION("fr", "GlobalNamespace", "Health", "Sant");
			TEST_TRANSLATION("de", "GlobalNamespace", "Health", "Gesundheit");
		});

		// ====================== 3. TESTS: VALID CSV WITH NAMESPACE AND DEVNOTES AND STRING TABLE ======================
		It(TEXT("VALID CSV WITH NAMESPACE AND DEVNOTES AND STRING TABLE"), [this]()
		{
			const FString CSV = TEXT("namespace,devnotes,key,lang-en,lang-pl,lang-fr,lang-de\n")
				TEXT("MainMenu,DevNote_1,StartGame,Start Game,Rozpocznij gre,Commencer le jeu,Spiel starten\n")
				TEXT("MainMenu,DevNote_2,QuitGame,Quit,Wyjd ,Quitter,Beenden\n")
				TEXT("MainMenu,DevNote_3,Settings,Settings,Ustawienia,Paramtres,Einstellungen\n")
				TEXT("MainMenu,DevNote_4,Credits,Credits,Napisy,Cr dits,Abspann\n")
				TEXT("Gameplay,DevNote_5,Health,Health,Zdrowie,Sant,Gesundheit\n")
				TEXT("Gameplay,DevNote_6,Damage,Damage,Obra enia,D g ts,Schaden\n")
				TEXT("Gameplay,DevNote_7,Score,Score,Wynik,Score,Punktzahl\n")
				TEXT("Gameplay,DevNote_8,Level,Level,Poziom,Niveau,Stufe\n");

				TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(CSVName, CSV));

				FString OutMessage;
				const bool bSuccess = UELTEditor::GenerateLocFilesImpl(
					FELTAutomationCommon::GetTestCSVPath(CSVName),
					LocPath,
					LocName,
					TEXT("GlobalNamespace"), // Global namespace
					TEXT(","), // Separator
					TEXT("NONE"), // Empty value fallback
					true, // Generate String Tables
					OutMessage); // Out Message

				TestTrue(TEXT("GenerateLocFilesImpl succeeded"), bSuccess);

				TEST_LOCFILE_EXISTS("en");
				TEST_LOCFILE_EXISTS("pl");
				TEST_LOCFILE_EXISTS("fr");
				TEST_LOCFILE_EXISTS("de");

				TEST_TRANSLATION("en", "MainMenu", "StartGame", "Start Game");
				TEST_TRANSLATION("pl", "MainMenu", "StartGame", "Rozpocznij gre");
				TEST_TRANSLATION("fr", "MainMenu", "StartGame", "Commencer le jeu");
				TEST_TRANSLATION("de", "MainMenu", "StartGame", "Spiel starten");

				TEST_TRANSLATION("en", "Gameplay", "Health", "Health");
				TEST_TRANSLATION("pl", "Gameplay", "Health", "Zdrowie");
				TEST_TRANSLATION("fr", "Gameplay", "Health", "Sant");
				TEST_TRANSLATION("de", "Gameplay", "Health", "Gesundheit");

				TEST_STRING_TABLE_EXISTS("MainMenu");
				TEST_STRING_TABLE_EXISTS("Gameplay");

				TEST_STRING_TABLE("en", "MainMenu", "StartGame", "Start Game");
				TEST_STRING_TABLE("pl", "MainMenu", "StartGame", "Rozpocznij gre");
				TEST_STRING_TABLE("fr", "Gameplay", "Health", "Sant");
				TEST_STRING_TABLE("de", "Gameplay", "Health", "Gesundheit");

				// For 5.8 and later - test if devnotes are properly imported into string table metadata
				/*
				FString StringTableName = UELTEditor::GetStringTableName(LocName, TEXT("MainMenu"));
				TSharedPtr<FStringTable> StringTable = FStringTableRegistry::Get().FindStringTable(*StringTableName);
				TestTrue(TEXT("String Table found in registry"), StringTable.IsValid());
				FString DevNote;
				TestTrue(TEXT("DevNote_1 metadata exists for StartGame key"), StringTable->GetMetaData(TEXT("StartGame"), TEXT("DevNote_1"), DevNote));
				TestTrue(TEXT("DevNote_1 metadata value is correct"), DevNote.Equals(TEXT("DevNote_1")));
				*/
		});

		// ====================== 4. TESTS: VALID CSV WITH NAMESPACE AND DEVNOTES AND STRING TABLES IN OTHER ORDER ======================
		It(TEXT("VALID CSV WITH NAMESPACE AND DEVNOTES AND STRING TABLES IN OTHER ORDER"), [this]()
		{
			const FString CSV = TEXT("devnotes,namespace,key,lang-en,lang-pl,lang-fr,lang-de\n")
				TEXT("DevNote_1,MainMenu,StartGame,Start Game,Rozpocznij gre,Commencer le jeu,Spiel starten\n")
				TEXT("DevNote_2,MainMenu,QuitGame,Quit,Wyjd ,Quitter,Beenden\n")
				TEXT("DevNote_3,MainMenu,Settings,Settings,Ustawienia,Paramtres,Einstellungen\n")
				TEXT("DevNote_4,Credits,Credits,Napisy,Cr dits,Abspann,\n")
				TEXT("DevNote_5,Gameplay,Health,Health,Zdrowie,Sant,Gesundheit\n")
				TEXT("DevNote_6,Gameplay,Damage,Damage,Obra enia,D g ts,Schaden\n")
				TEXT("DevNote_7,Gameplay,Score,Score,Wynik,Score,Punktzahl\n")
				TEXT("DevNote_8,Gameplay,Level,Level,Poziom,Niveau,Stufe\n");

				TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(CSVName, CSV));

				FString OutMessage;
				const bool bSuccess = UELTEditor::GenerateLocFilesImpl(
					FELTAutomationCommon::GetTestCSVPath(CSVName),
					LocPath,
					LocName,
					TEXT("GlobalNamespace"), // Global namespace
					TEXT(","), // Separator
					TEXT("NONE"), // Empty value fallback
					true, // Generate String Tables
					OutMessage); // Out Message

				TestTrue(TEXT("GenerateLocFilesImpl succeeded"), bSuccess);

				TEST_LOCFILE_EXISTS("en");
				TEST_LOCFILE_EXISTS("pl");
				TEST_LOCFILE_EXISTS("fr");
				TEST_LOCFILE_EXISTS("de");

				TEST_TRANSLATION("en", "MainMenu", "StartGame", "Start Game");
				TEST_TRANSLATION("pl", "MainMenu", "StartGame", "Rozpocznij gre");
				TEST_TRANSLATION("fr", "MainMenu", "StartGame", "Commencer le jeu");
				TEST_TRANSLATION("de", "MainMenu", "StartGame", "Spiel starten");

				TEST_TRANSLATION("en", "Gameplay", "Health", "Health");
				TEST_TRANSLATION("pl", "Gameplay", "Health", "Zdrowie");
				TEST_TRANSLATION("fr", "Gameplay", "Health", "Sant");
				TEST_TRANSLATION("de", "Gameplay", "Health", "Gesundheit");

				TEST_STRING_TABLE_EXISTS("MainMenu");
				TEST_STRING_TABLE_EXISTS("Gameplay");

				TEST_STRING_TABLE("en", "MainMenu", "StartGame", "Start Game");
				TEST_STRING_TABLE("pl", "MainMenu", "StartGame", "Rozpocznij gre");
				TEST_STRING_TABLE("fr", "Gameplay", "Health", "Sant");
				TEST_STRING_TABLE("de", "Gameplay", "Health", "Gesundheit");

				// For 5.8 and later - test if devnotes are properly imported into string table metadata
				/*
				FString StringTableName = UELTEditor::GetStringTableName(LocName, TEXT("MainMenu"));
				TSharedPtr<FStringTable> StringTable = FStringTableRegistry::Get().FindStringTable(*StringTableName);
				TestTrue(TEXT("String Table found in registry"), StringTable.IsValid());
				FString DevNote;
				TestTrue(TEXT("DevNote_1 metadata exists for StartGame key"), StringTable->GetMetaData(TEXT("StartGame"), TEXT("DevNote_1"), DevNote));
				TestTrue(TEXT("DevNote_1 metadata value is correct"), DevNote.Equals(TEXT("DevNote_1")));
				*/
		});

		// ====================== 5. TESTS: INVALID CSV WITH NAMESPACE AND DEVNOTES IN BAD PLACE ======================
		It(TEXT("INVALID CSV WITH NAMESPACE AND DEVNOTES IN BAD PLACE"), [this]()
		{
			const FString CSV = TEXT("devnotes,key,namespace,lang-en,lang-pl,lang-fr,lang-de\n")
				TEXT("DevNote_1,StartGame,MainMenu,Start Game,Rozpocznij gre,Commencer le jeu,Spiel starten\n")
				TEXT("DevNote_2,QuitGame,MainMenu,Quit,Wyjd ,Quitter,Beenden\n")
				TEXT("DevNote_3,Settings,MainMenu,Settings,Ustawienia,Paramtres,Einstellungen\n")
				TEXT("DevNote_4,MainMenu,Credits,Napisy,Cr dits,Abspann\n")
				TEXT("DevNote_5,Health,Gameplay,Health,Zdrowie,Sant,Gesundheit\n")
				TEXT("DevNote_6,Damage,Gameplay,Damage,Obra enia,D g ts,Schaden\n")
				TEXT("DevNote_7,Score,Gameplay,Score,Wynik,Score,Punktzahl\n")
				TEXT("DevNote_8,Level,Gameplay,Level,Poziom,Niveau,Stufe\n");

				TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(CSVName, CSV));

				FString OutMessage;
				const bool bSuccess = UELTEditor::GenerateLocFilesImpl(
					FELTAutomationCommon::GetTestCSVPath(CSVName),
					LocPath,
					LocName,
					TEXT("GlobalNamespace"), // Global namespace
					TEXT(","), // Separator
					TEXT("NONE"), // Empty value fallback
					false, // Generate String Tables
					OutMessage); // Out Message

				TestFalse(TEXT("GenerateLocFilesImpl failed"), bSuccess);
		});

		// ====================== 6. TESTS: INVALID CSV WITHOUT NAMESPACE AND NO GLOBAL NAMESPACE ======================
		It(TEXT("INVALID CSV WITHOUT NAMESPACE AND NO GLOBAL NAMESPACE"), [this]()
		{
			const FString CSV = TEXT("key,lang-en,lang-pl,lang-fr,lang-de\n")
				TEXT("StartGame,Start Game,Rozpocznij gre,Commencer le jeu,Spiel starten\n")
				TEXT("QuitGame,Quit,Wyjd ,Quitter,Beenden\n")
				TEXT("Settings,Settings,Ustawienia,Paramtres,Einstellungen\n")
				TEXT("Credits,Credits,Napisy,Cr dits,Abspann\n")
				TEXT("Health,Health,Zdrowie,Sant,Gesundheit\n")
				TEXT("Damage,Damage,Obra enia,D g ts,Schaden\n")
				TEXT("Score,Score,Wynik,Score,Punktzahl\n")
				TEXT("Level,Level,Poziom,Niveau,Stufe\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(CSVName, CSV));

			FString OutMessage;
			const bool bSuccess = UELTEditor::GenerateLocFilesImpl(
				FELTAutomationCommon::GetTestCSVPath(CSVName),
				LocPath,
				LocName,
				TEXT(""), // Global namespace
				TEXT(","), // Separator
				TEXT("NONE"), // Empty value fallback
				false, // Generate String Tables
				OutMessage); // Out Message

			TestFalse(TEXT("GenerateLocFilesImpl failed"), bSuccess);
		});

		// ====================== 7. TESTS: VALID CSV WITH NAMESPACE AND EMPTY VALUES WITH FALLBACK ======================
		It(TEXT("VALID CSV WITH NAMESPACE AND EMPTY VALUES WITH FALLBACK"), [this]()
		{
			const FString CSV = TEXT("devnotes,namespace,key,lang-en,lang-ja,lang-zh\n")
				TEXT("Button prompt for attacking,Combat,Attack,Attack,,\n")
				TEXT("Button prompt for defending,Combat,Defend,Defend,,\n")
				TEXT("Trigger after 3 hits,Combat,Special,Special Attack,,\n")
				TEXT("Rare weapon with blue aura,Items,Sword,Legendary Sword,,\n")
				TEXT("Used to reduce damage intake,Items,Shield,Ancient Shield,,\n")
				TEXT("Restores 50% of HP,Items,Potion,Health Potion,,\n")
				TEXT("Increase intel by 10,Items,Scroll,Scroll of Wisdom,,\n")
				TEXT("First NPC interaction,Dialogue,Greeting,Greetings traveler,,\n");

				TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(CSVName, CSV));

				FString OutMessage;
				const bool bSuccess = UELTEditor::GenerateLocFilesImpl(
					FELTAutomationCommon::GetTestCSVPath(CSVName),
					LocPath,
					LocName,
					TEXT("GlobalNamespace"), // Global namespace
					TEXT(","), // Separator
					TEXT("FIRST_LANG"), // Fallback
					true, // Generate String Tables
					OutMessage); // Out Message

				TestTrue(TEXT("GenerateLocFilesImpl succeeded"), bSuccess);

				TEST_LOCFILE_EXISTS("en");
				TEST_LOCFILE_EXISTS("ja");
				TEST_LOCFILE_EXISTS("zh");

				TEST_TRANSLATION("en", "Combat", "Attack", "Attack");
				TEST_TRANSLATION("ja", "Items", "Sword", "Legendary Sword");
				TEST_TRANSLATION("zh", "Dialogue", "Greeting", "Greetings traveler");

				TEST_STRING_TABLE_EXISTS("Combat");
				TEST_STRING_TABLE_EXISTS("Items");
				TEST_STRING_TABLE_EXISTS("Dialogue");

				TEST_STRING_TABLE("en", "Combat", "Attack", "Attack");
				TEST_STRING_TABLE("ja", "Items", "Sword", "Legendary Sword");
				TEST_STRING_TABLE("zh", "Dialogue", "Greeting", "Greetings traveler");
		});

		// ====================== 8. TESTS: VALID CSV WITH NAMESPACE AND EMPTY VALUES WITH FALLBACK ======================
		It(TEXT("VALID CSV WITH NAMESPACE AND EMPTY VALUES WITHOUT FALLBACK"), [this]()
		{
			const FString CSV = TEXT("devnotes,namespace,key,lang-en,lang-ja,lang-zh\n")
				TEXT("Button prompt for attacking,Combat,Attack,Attack,,\n")
				TEXT("Button prompt for defending,Combat,Defend,Defend,,\n")
				TEXT("Trigger after 3 hits,Combat,Special,Special Attack,,\n")
				TEXT("Rare weapon with blue aura,Items,Sword,Legendary Sword,,\n")
				TEXT("Used to reduce damage intake,Items,Shield,Ancient Shield,,\n")
				TEXT("Restores 50% of HP,Items,Potion,Health Potion,,\n")
				TEXT("Increase intel by 10,Items,Scroll,Scroll of Wisdom,,\n")
				TEXT("First NPC interaction,Dialogue,Greeting,Greetings traveler,,\n");

				TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(CSVName, CSV));

				FString OutMessage;
				const bool bSuccess = UELTEditor::GenerateLocFilesImpl(
					FELTAutomationCommon::GetTestCSVPath(CSVName),
					LocPath,
					LocName,
					TEXT("GlobalNamespace"), // Global namespace
					TEXT(","), // Separator
					TEXT("NONE"), // Fallback
					true, // Generate String Tables
					OutMessage); // Out Message

				TestTrue(TEXT("GenerateLocFilesImpl succeeded"), bSuccess);

				TEST_LOCFILE_EXISTS("en");
				TEST_LOCFILE_EXISTS("ja");
				TEST_LOCFILE_EXISTS("zh");

				TEST_TRANSLATION("en", "Combat", "Attack", "Attack");
				TEST_TRANSLATION("ja", "Items", "Sword", "");
				TEST_TRANSLATION("zh", "Dialogue", "Greeting", "");

				TEST_STRING_TABLE_EXISTS("Combat");
				TEST_STRING_TABLE_EXISTS("Items");
				TEST_STRING_TABLE_EXISTS("Dialogue");

				TEST_STRING_TABLE("en", "Combat", "Attack", "Attack");
				TEST_STRING_TABLE("ja", "Items", "Sword", "");
				TEST_STRING_TABLE("zh", "Dialogue", "Greeting", "");
		});

		// ====================== 9. TESTS: VALID CSV WITH NAMESPACE AND EMPTY VALUES WITH KEY FALLBACK ======================
		It(TEXT("VALID CSV WITH NAMESPACE AND EMPTY VALUES WITH KEY FALLBACK"), [this]()
		{
			const FString CSV = TEXT("devnotes,namespace,key,lang-en,lang-ja,lang-zh\n")
				TEXT("Button prompt for attacking,Combat,Attack,Attack,,\n")
				TEXT("Button prompt for defending,Combat,Defend,Defend,,\n")
				TEXT("Trigger after 3 hits,Combat,Special,Special Attack,,\n")
				TEXT("Rare weapon with blue aura,Items,Sword,Legendary Sword,,\n")
				TEXT("Used to reduce damage intake,Items,Shield,Ancient Shield,,\n")
				TEXT("Restores 50% of HP,Items,Potion,Health Potion,,\n")
				TEXT("Increase intel by 10,Items,Scroll,Scroll of Wisdom,,\n")
				TEXT("First NPC interaction,Dialogue,Greeting,Greetings traveler,,\n");

				TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(CSVName, CSV));

				FString OutMessage;
				const bool bSuccess = UELTEditor::GenerateLocFilesImpl(
					FELTAutomationCommon::GetTestCSVPath(CSVName),
					LocPath,
					LocName,
					TEXT("GlobalNamespace"), // Global namespace
					TEXT(","), // Separator
					TEXT("KEY"), // Fallback
					true, // Generate String Tables
					OutMessage); // Out Message

				TestTrue(TEXT("GenerateLocFilesImpl succeeded"), bSuccess);

				TEST_LOCFILE_EXISTS("en");
				TEST_LOCFILE_EXISTS("ja");
				TEST_LOCFILE_EXISTS("zh");

				TEST_TRANSLATION("en", "Combat", "Attack", "Attack");
				TEST_TRANSLATION("ja", "Items", "Sword", "Sword");
				TEST_TRANSLATION("zh", "Dialogue", "Greeting", "Greeting");

				TEST_STRING_TABLE_EXISTS("Combat");
				TEST_STRING_TABLE_EXISTS("Items");
				TEST_STRING_TABLE_EXISTS("Dialogue");

				TEST_STRING_TABLE("en", "Combat", "Attack", "Attack");
				TEST_STRING_TABLE("ja", "Items", "Sword", "Sword");
				TEST_STRING_TABLE("zh", "Dialogue", "Greeting", "Greeting");
		});

		// ====================== 10. TESTS: CSV CONVERT UNDERSCORES TO LINES ======================
		It(TEXT("CSV CONVERT UNDERSCORES TO LINES"), [this]()
		{
			const FString CSV = TEXT("namespace,key,lang_en,lang_pt_br,lang_zh_cn\n")
				TEXT("UI,ButtonLabel,Click Me,Clique em Mim,\n")
				TEXT("UI,ErrorMessage,An error occurred,Ocorreu um erro,\n")
				TEXT("Settings,Volume,Volume,Volume,\n")
				TEXT("Settings,Brightness,Brightness,Brilho,\n")
				TEXT("Audio,MusicVolume,Music,M sica,\n")
				TEXT("Audio,SFXVolume,Sound Effects,Efeitos Sonoros,\n")
				TEXT("Audio,Mute,Mute,Silenciar,\n")
				TEXT("Audio,Unmute,Unmute,Desmutizar,\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(CSVName, CSV));

			FString OutMessage;
			const bool bSuccess = UELTEditor::GenerateLocFilesImpl(
				FELTAutomationCommon::GetTestCSVPath(CSVName),
				LocPath,
				LocName,
				TEXT("GlobalNamespace"), // Global namespace
				TEXT(","), // Separator
				TEXT("FIRST_LANG"), // Fallback
				true, // Generate String Tables
				OutMessage); // Out Message

			TestTrue(TEXT("GenerateLocFilesImpl succeeded"), bSuccess);

			TEST_LOCFILE_EXISTS("en");
			TEST_LOCFILE_EXISTS("pt-br");
			TEST_LOCFILE_EXISTS("zh-cn");

			TEST_TRANSLATION("en", "UI", "ButtonLabel", "Click Me");
			TEST_TRANSLATION("pt-br", "Audio", "MusicVolume", "M sica");
			TEST_TRANSLATION("zh-cn", "Settings", "Brightness", "Brightness");			
		});

		// ====================== 11. TESTS: INVALID CSV DUPLICATED COLUMNS ======================
		It(TEXT("INVALID CSV DUPLICATED COLUMNS"), [this]()
		{
			const FString CSV = TEXT("namespace,key,namespace,lang-en\n")
				TEXT("Menu,Start,Menu,Start Game\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(CSVName, CSV));

			FString OutMessage;
			const bool bSuccess = UELTEditor::GenerateLocFilesImpl(
				FELTAutomationCommon::GetTestCSVPath(CSVName),
				LocPath,
				LocName,
				TEXT(""), // Global namespace
				TEXT(","), // Separator
				TEXT("NONE"), // Empty value fallback
				false, // Generate String Tables
				OutMessage); // Out Message

			TestFalse(TEXT("GenerateLocFilesImpl failed"), bSuccess);
		});

		// ====================== 12. TESTS: INVALID CSV NO KEY COLUMNS ======================
		It(TEXT("INVALID CSV NO KEY COLUMNS"), [this]()
		{
			const FString CSV = TEXT("namespace,value,lang-en,lang-fr\n")
				TEXT("MainMenu,Start Game,Commencer\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(CSVName, CSV));

			FString OutMessage;
			const bool bSuccess = UELTEditor::GenerateLocFilesImpl(
				FELTAutomationCommon::GetTestCSVPath(CSVName),
				LocPath,
				LocName,
				TEXT(""), // Global namespace
				TEXT(","), // Separator
				TEXT("NONE"), // Empty value fallback
				false, // Generate String Tables
				OutMessage); // Out Message

			TestFalse(TEXT("GenerateLocFilesImpl failed"), bSuccess);
		});

		// ====================== 13. TESTS: INVALID CSV NO LANG COLUMNS ======================
		It(TEXT("INVALID CSV NO LANG COLUMNS"), [this]()
		{
			const FString CSV = TEXT("namespace,key\n")
				TEXT("MainMenu,Start\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(CSVName, CSV));

			FString OutMessage;
			const bool bSuccess = UELTEditor::GenerateLocFilesImpl(
				FELTAutomationCommon::GetTestCSVPath(CSVName),
				LocPath,
				LocName,
				TEXT(""), // Global namespace
				TEXT(","), // Separator
				TEXT("NONE"), // Empty value fallback
				false, // Generate String Tables
				OutMessage); // Out Message

			TestFalse(TEXT("GenerateLocFilesImpl failed"), bSuccess);
		});

		// ====================== 14. TESTS: INVALID CSV INVALID NUM OF COLUMNS ======================
		It(TEXT("INVALID CSV INVALID NUM OF COLUMNS"), [this]()
		{
			const FString CSV = TEXT("namespace,key,lang-en,lang-de\n")
				TEXT("Menu,Start,Start Game,Spiel starten\n")
				TEXT("Menu,Quit,Quit\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(CSVName, CSV));

			FString OutMessage;
			const bool bSuccess = UELTEditor::GenerateLocFilesImpl(
				FELTAutomationCommon::GetTestCSVPath(CSVName),
				LocPath,
				LocName,
				TEXT(""), // Global namespace
				TEXT(","), // Separator
				TEXT("NONE"), // Empty value fallback
				false, // Generate String Tables
				OutMessage); // Out Message

			TestFalse(TEXT("GenerateLocFilesImpl failed"), bSuccess);
		});

		// ====================== 15. TESTS: INVALID CSV INVALID SEPARATOR ======================
		It(TEXT("INVALID CSV INVALID SEPARATOR"), [this]()
		{
			FString CSV = TEXT("namespace,key,lang-en,lang-de\n")
				TEXT("Menu,Start,Start Game,Spiel starten\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(CSVName, CSV));

			FString OutMessage;
			const bool bSuccess = UELTEditor::GenerateLocFilesImpl(
				FELTAutomationCommon::GetTestCSVPath(CSVName),
				LocPath,
				LocName,
				TEXT(""), // Global namespace
				TEXT(";"), // Separator
				TEXT("NONE"), // Empty value fallback
				false, // Generate String Tables
				OutMessage); // Out Message

			TestFalse(TEXT("GenerateLocFilesImpl failed"), bSuccess);
		});

		// ====================== 16. TESTS: CSV WITH SEMICOLON SEPARATOR ======================
		It(TEXT("CSV WITH SEMICOLON SEPARATOR"), [this]()
		{
			const FString CSV = TEXT("namespace;key;lang-en;lang-sv;lang-no\n")
				TEXT("Navigation;North;North;North_SV;North_NO\n")
				TEXT("Navigation;South;South;South_SV;South_NO\n")
				TEXT("Navigation;East;East;East_SV;East_NO\n")
				TEXT("Navigation;West;West;West_SV;West_NO\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(CSVName, CSV));

			FString OutMessage;
			const bool bSuccess = UELTEditor::GenerateLocFilesImpl(
				FELTAutomationCommon::GetTestCSVPath(CSVName),
				LocPath,
				LocName,
				TEXT("GlobalNamespace"),
				TEXT(";"),
				TEXT("NONE"),
				false,
				OutMessage);

			TestTrue(TEXT("GenerateLocFilesImpl succeeded"), bSuccess);

			TEST_LOCFILE_EXISTS("en");
			TEST_LOCFILE_EXISTS("sv");
			TEST_LOCFILE_EXISTS("no");

			TEST_TRANSLATION("en", "Navigation", "North", "North");
			TEST_TRANSLATION("sv", "Navigation", "North", "North_SV");
			TEST_TRANSLATION("no", "Navigation", "East", "East_NO");
		});

		// ====================== 17. TESTS: CSV WITH TAB SEPARATOR ======================
		It(TEXT("CSV WITH TAB SEPARATOR"), [this]()
		{
			const FString CSV = TEXT("namespace\tkey\tlang-en\tlang-da\tlang-fi\n")
				TEXT("Weather\tSunny\tSunny\tSunny_DA\tSunny_FI\n")
				TEXT("Weather\tRainy\tRainy\tRainy_DA\tRainy_FI\n")
				TEXT("Weather\tSnowy\tSnowy\tSnowy_DA\tSnowy_FI\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(CSVName, CSV));

			FString OutMessage;
			const bool bSuccess = UELTEditor::GenerateLocFilesImpl(
				FELTAutomationCommon::GetTestCSVPath(CSVName),
				LocPath,
				LocName,
				TEXT("GlobalNamespace"),
				TEXT("\t"),
				TEXT("NONE"),
				false,
				OutMessage);

			TestTrue(TEXT("GenerateLocFilesImpl succeeded"), bSuccess);

			TEST_LOCFILE_EXISTS("en");
			TEST_LOCFILE_EXISTS("da");
			TEST_LOCFILE_EXISTS("fi");

			TEST_TRANSLATION("en", "Weather", "Sunny", "Sunny");
			TEST_TRANSLATION("da", "Weather", "Rainy", "Rainy_DA");
			TEST_TRANSLATION("fi", "Weather", "Snowy", "Snowy_FI");
		});

		// ====================== 17. TESTS: CSV WITH MANY LANGUAGES ======================
		It(TEXT("CSV WITH MANY LANGUAGES"), [this]()
		{
			const FString CSV = TEXT("namespace,key,lang-en,lang-es,lang-fr,lang-de,lang-it,lang-pt,lang-ja\n")
				TEXT("Status,Alive,Alive,Alive_ES,Alive_FR,Alive_DE,Alive_IT,Alive_PT,Alive_JA\n")
				TEXT("Status,Dead,Dead,Dead_ES,Dead_FR,Dead_DE,Dead_IT,Dead_PT,Dead_JA\n")
				TEXT("Status,Sleeping,Sleeping,Sleeping_ES,Sleeping_FR,Sleeping_DE,Sleeping_IT,Sleeping_PT,Sleeping_JA\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(CSVName, CSV));

			FString OutMessage;
			const bool bSuccess = UELTEditor::GenerateLocFilesImpl(
				FELTAutomationCommon::GetTestCSVPath(CSVName),
				LocPath,
				LocName,
				TEXT("GlobalNamespace"),
				TEXT(","),
				TEXT("NONE"),
				false,
				OutMessage);

			TestTrue(TEXT("GenerateLocFilesImpl succeeded"), bSuccess);

			TEST_LOCFILE_EXISTS("en");
			TEST_LOCFILE_EXISTS("es");
			TEST_LOCFILE_EXISTS("fr");
			TEST_LOCFILE_EXISTS("de");
			TEST_LOCFILE_EXISTS("it");
			TEST_LOCFILE_EXISTS("pt");
			TEST_LOCFILE_EXISTS("ja");

			TEST_TRANSLATION("en", "Status", "Alive", "Alive");
			TEST_TRANSLATION("es", "Status", "Dead", "Dead_ES");
			TEST_TRANSLATION("fr", "Status", "Sleeping", "Sleeping_FR");
			TEST_TRANSLATION("de", "Status", "Alive", "Alive_DE");
			TEST_TRANSLATION("it", "Status", "Dead", "Dead_IT");
			TEST_TRANSLATION("pt", "Status", "Sleeping", "Sleeping_PT");
			TEST_TRANSLATION("ja", "Status", "Alive", "Alive_JA");
		});

		// ====================== 18. TESTS: Util functions ======================
		It(TEXT("UTILS FUNCTIONS"), [this]()
		{
			const FString CSV = TEXT("namespace,key,lang-en,lang-pl,lang-fr,lang-de\n")
				TEXT("MainMenu,StartGame,Start Game,Rozpocznij gre,Commencer le jeu,Spiel starten\n")
				TEXT("MainMenu,QuitGame,Quit,Wyjd ,Quitter,Beenden\n")
				TEXT("MainMenu,Settings,Settings,Ustawienia,Paramtres,Einstellungen\n")
				TEXT("MainMenu,Credits,Credits,Napisy,Cr dits,Abspann\n")
				TEXT("Gameplay,Health,Health,Zdrowie,Sant,Gesundheit\n")
				TEXT("Gameplay,Damage,Damage,Obra enia,D g ts,Schaden\n")
				TEXT("Gameplay,Score,Score,Wynik,Score,Punktzahl\n")
				TEXT("Gameplay,Level,Level,Poziom,Niveau,Stufe\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(CSVName, CSV));

			FString OutMessage;
			const bool bSuccess = UELTEditor::GenerateLocFilesImpl(
				FELTAutomationCommon::GetTestCSVPath(CSVName), 
				LocPath,
				LocName, 
				TEXT("GlobalNamespace"), // Global namespace
				TEXT(","), // Separator
				TEXT("NONE"), // Empty value fallback
				false, // Generate String Tables
				OutMessage); // Out Message

			TestTrue(TEXT("GenerateLocFilesImpl succeeded"), bSuccess);

			TEST_LOCFILE_EXISTS("en");
			TEST_LOCFILE_EXISTS("pl");
			TEST_LOCFILE_EXISTS("fr");
			TEST_LOCFILE_EXISTS("de");

			FTextLocalizationManager::Get().DisableGameLocalizationPreview();
			FTextLocalizationManager::Get().EnableGameLocalizationPreview(TEXT("en"));

			const FText LocText = UELT::GetLocalizedText(TEXT("MainMenu"), TEXT("StartGame"));
			TestTrue(TEXT("LocText valid"), LocText.ToString().Equals(TEXT("Start Game")));

			const FString LocString = UELT::GetLocalizedString(TEXT("MainMenu"), TEXT("StartGame"));
			TestTrue(TEXT("LocString valid"), LocString.Equals(TEXT("Start Game")));

			TestTrue(TEXT("Validate Text"), UELTEditorUtils::ValidateText(LocText));

			{
				FString OutPackage, OutNamespace, OutKey, OutSource;
				UELTBlueprintLibrary::GetTextData(LocText, OutPackage, OutNamespace, OutKey, OutSource);
				TestTrue(TEXT("Text Namespace test"), OutNamespace.Equals(TEXT("MainMenu")));
				TestTrue(TEXT("Text Key test"), OutKey.Equals(TEXT("StartGame")));
				TestTrue(TEXT("Text Source test"), OutSource.Equals(TEXT("StartGame")));
			}
	
			FString Buffer = UELTBlueprintLibrary::GetTextAsBuffer(LocText, false, false);
			FText OtherText = UELTBlueprintLibrary::MakeTextFromBuffer(Buffer, false);
			TestTrue(TEXT("Text Buffer 1"), OtherText.ToString().Equals(LocText.ToString()));
			TestTrue(TEXT("Text Buffer 2"), UELTBlueprintLibrary::AreTextKeysEqual(LocText, OtherText));
		});

		AfterEach([]()
		{
			FELTAutomationCommon::CleanTestDirectory();
		});
	});
}

ELTTESTS_PRAGMA_ENABLE_OPTIMIZATION
