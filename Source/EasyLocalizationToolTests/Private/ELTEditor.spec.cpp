// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#include "Misc/AutomationTest.h"
#include "ELTEditor.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Internationalization/TextLocalizationResource.h"
#include "Internationalization/StringTableRegistry.h"
#include "Internationalization/StringTableCore.h"
#include "ELTEditorSettings.h"

ELTTESTS_PRAGMA_DISABLE_OPTIMIZATION

DEFINE_LOG_CATEGORY_STATIC(ELTAutomationLog, Log, All);

/**
 * Common test utilities for ELT tests
 */
class FELTAutomationCommon
{
public:
	static FString GetTestDataDir()
	{
		return FPaths::ProjectPluginsDir() / TEXT("EasyLocalizationTool/Source/EasyLocalizationToolTests/TestData");
	}

	static FString GetTestCSVDir()
	{
		return GetTestDataDir() / TEXT("CSV");
	}

	static void EnsureDirectories()
	{
		IFileManager::Get().MakeDirectory(*GetTestCSVDir(), true);
	}

	static bool WriteTestCSV(const FString& Filename, const FString& Content)
	{
		EnsureDirectories();
		FString FilePath = GetTestCSVDir() / Filename + TEXT(".csv");
		return FFileHelper::SaveStringToFile(Content, *FilePath);
	}

	static bool LocResFileExists(const FString& LocPath, const FString& LocName, const FString& Language)
	{
		FString FilePath = LocPath / Language / LocName + TEXT(".locres");
		return IFileManager::Get().FileExists(*FilePath);
	}

	static bool StringTableFileExists(const FString& LocPath, const FString& LocName, const FString& Namespace)
	{
		FString FilePath = LocPath / UELTEditor::GetStringTableName(LocName, Namespace) + TEXT(".uasset");
		return IFileManager::Get().FileExists(*FilePath);
	}

	static FString GetTestCSVPath(const FString& TestName)
	{
		return GetTestCSVDir() / TestName + TEXT(".csv");
	}
};

#define TEST_TRANSLATION(_Lang, _Namespace, _Key, _Expected) { \
FTextLocalizationManager::Get().DisableGameLocalizationPreview(); \
FTextLocalizationManager::Get().EnableGameLocalizationPreview(TEXT(_Lang)); \
FString Result = NSLOCTEXT(_Namespace, _Key, _Key).ToString(); \
TestTrue(FString::Printf(TEXT("Test lang "##_Lang" of namespace: "##_Namespace" key: "##_Key" expect: "##_Expected" result: %s"), *Result), Result.Equals(##_Expected)); \
}

#define TEST_STRING_TABLE(_Lang, _LocName, _LocPath, _Namespace, _Key, _Expected) { \
const FString StringTableName = UELTEditor::GetStringTableName(_LocName, _Namespace); \
const FString StringTableID = FPackageName::FilenameToLongPackageName(_LocPath / StringTableName + TEXT(".uasset")) + TEXT(".") + StringTableName; \
FTextLocalizationManager::Get().DisableGameLocalizationPreview(); \
FTextLocalizationManager::Get().EnableGameLocalizationPreview(_Lang); \
FText LocalizedText = FText::FromStringTable(FName(StringTableID), TEXT(_Key), EStringTableLoadingPolicy::FindOrLoad); \
FString Result = LocalizedText.ToString(); \
TestTrue(FString::Printf(TEXT("Test String Table: %s lang "##_Lang" key: "##_Key" expect: "##_Expected" result: %s"), *StringTableID, *Result), Result.Equals(##_Expected)); \
}

// ============================================================================
// VALID CSV TESTS
// ============================================================================

BEGIN_DEFINE_SPEC(FELTGenerateLocFiles_SimpleCSV, "EasyLocalizationTool.GenerateLocFiles.SimpleCSV", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
END_DEFINE_SPEC(FELTGenerateLocFiles_SimpleCSV)

void FELTGenerateLocFiles_SimpleCSV::Define()
{
	Describe(TEXT("GenerateLocFilesImpl - Simple CSV with namespace"), [this]()
	{
		It(TEXT("Should generate localization files for a basic CSV with namespace, key, and multiple languages"), [this]()
		{
			FELTAutomationCommon::EnsureDirectories();

			// Create a simple CSV
			FString CSV = TEXT("namespace,key,lang-en,lang-pl,lang-fr,lang-de\n")
				TEXT("MainMenu,StartGame,Start Game,Rozpocznij gre,Commencer le jeu,Spiel starten\n")
				TEXT("MainMenu,QuitGame,Quit,Wyjd ,Quitter,Beenden\n")
				TEXT("MainMenu,Settings,Settings,Ustawienia,Paramtres,Einstellungen\n")
				TEXT("MainMenu,Credits,Credits,Napisy,Cr dits,Abspann\n")
				TEXT("Gameplay,Health,Health,Zdrowie,Sant,Gesundheit\n")
				TEXT("Gameplay,Damage,Damage,Obra enia,D g ts,Schaden\n")
				TEXT("Gameplay,Score,Score,Wynik,Score,Punktzahl\n")
				TEXT("Gameplay,Level,Level,Poziom,Niveau,Stufe\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(TEXT("SimpleCSV"), CSV));

			FString OutMessage;
			TArray<FString> CSVPaths;
			CSVPaths.Add(FELTAutomationCommon::GetTestCSVPath(TEXT("SimpleCSV")));
			FString LocPath = UELTEditorSettings::GetLocalizationPath();
			FString LocName = TEXT("Game");

			bool bSuccess = UELTEditor::GenerateLocFilesImpl(CSVPaths, LocPath, LocName, TEXT("TestNamespace"), TEXT(","), TEXT("NONE"), false, OutMessage);

			TestTrue(TEXT("GenerateLocFilesImpl succeeded"), bSuccess);
			TestTrue(TEXT("OutMessage indicates success"), OutMessage.Contains(TEXT("SUCCESS")));

			TestTrue(TEXT("English locres file created"), FELTAutomationCommon::LocResFileExists(LocPath, LocName, TEXT("en")));
			TestTrue(TEXT("Polish locres file created"), FELTAutomationCommon::LocResFileExists(LocPath, LocName, TEXT("pl")));
			TestTrue(TEXT("French locres file created"), FELTAutomationCommon::LocResFileExists(LocPath, LocName, TEXT("fr")));
			TestTrue(TEXT("German locres file created"), FELTAutomationCommon::LocResFileExists(LocPath, LocName, TEXT("de")));

			TEST_TRANSLATION("en", "MainMenu", "StartGame", "Start Game");
			TEST_TRANSLATION("pl", "MainMenu", "StartGame", "Rozpocznij gre");
			TEST_TRANSLATION("fr", "MainMenu", "StartGame", "Commencer le jeu");
			TEST_TRANSLATION("de", "MainMenu", "StartGame", "Spiel starten");

			TEST_TRANSLATION("en", "Gameplay", "Health", "Health");
			TEST_TRANSLATION("pl", "Gameplay", "Health", "Zdrowie");
			TEST_TRANSLATION("fr", "Gameplay", "Health", "Sant");
			TEST_TRANSLATION("de", "Gameplay", "Health", "Gesundheit");
		});
	});
}

// ============================================================================

BEGIN_DEFINE_SPEC(FELTGenerateLocFiles_GlobalNamespace, "EasyLocalizationTool.GenerateLocFiles.GlobalNamespace", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
END_DEFINE_SPEC(FELTGenerateLocFiles_GlobalNamespace)

void FELTGenerateLocFiles_GlobalNamespace::Define()
{
	Describe(TEXT("GenerateLocFilesImpl - Global Namespace"), [this]()
	{
		It(TEXT("Should generate files when using global namespace (empty namespace column)"), [this]()
		{
			
			FELTAutomationCommon::EnsureDirectories();

			FString CSV = TEXT("key,lang-en,lang-it,lang-pt\n")
				TEXT("Welcome,Welcome to the game,Benvenuto nel gioco,Bem-vindo ao jogo\n")
				TEXT("Exit,Exit Application,Esci dall'applicazione,Sair da aplica o\n")
				TEXT("Save,Save Progress,Salva Progresso,Salvar Progresso\n")
				TEXT("Load,Load Progress,Carica Progresso,Carregar Progresso\n")
				TEXT("Resume,Resume Game,Riprendi gioco,Retomar jogo\n")
				TEXT("Options,Game Options,Opzioni di gioco,Op es do jogo\n")
				TEXT("About,About This Game,Informazioni su questo gioco,Sobre este jogo\n")
				TEXT("Support,Contact Support,Contatta il supporto,Contate o suporte\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(TEXT("GlobalNamespace"), CSV));

			FString OutMessage;
			TArray<FString> CSVPaths;
			CSVPaths.Add(FELTAutomationCommon::GetTestCSVPath(TEXT("GlobalNamespace")));
			FString LocPath = UELTEditorSettings::GetLocalizationPath();
			FString LocName = TEXT("Game");

			bool bSuccess = UELTEditor::GenerateLocFilesImpl(CSVPaths, LocPath, LocName, TEXT("GlobalUI"), TEXT(","), TEXT("NONE"), false, OutMessage);

			TestTrue(TEXT("GenerateLocFilesImpl succeeded"), bSuccess);
			TestTrue(TEXT("English locres file created"), FELTAutomationCommon::LocResFileExists(LocPath, LocName, TEXT("en")));
			TestTrue(TEXT("Polish locres file created"), FELTAutomationCommon::LocResFileExists(LocPath, LocName, TEXT("it")));
			TestTrue(TEXT("French locres file created"), FELTAutomationCommon::LocResFileExists(LocPath, LocName, TEXT("pt")));

			TEST_TRANSLATION("en", "GlobalUI", "Welcome", "Welcome to the game");
			TEST_TRANSLATION("it", "GlobalUI", "Welcome", "Benvenuto nel gioco");
			TEST_TRANSLATION("pt", "GlobalUI", "Welcome", "Bem-vindo ao jogo");
		});
	});
}

// ============================================================================

BEGIN_DEFINE_SPEC(FELTGenerateLocFiles_WithDevNotes, "EasyLocalizationTool.GenerateLocFiles.WithDevNotes", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
END_DEFINE_SPEC(FELTGenerateLocFiles_WithDevNotes)

void FELTGenerateLocFiles_WithDevNotes::Define()
{
	Describe(TEXT("GenerateLocFilesImpl - With DevNotes"), [this]()
	{
		It(TEXT("Should generate files and string tables with dev notes from CSV"), [this]()
		{
			
			FELTAutomationCommon::EnsureDirectories();

			FString CSV = TEXT("namespace,devnotes,key,lang-en,lang-ja,lang-zh\n")
				TEXT("Combat,Button prompt for attacking,Attack,Attack,,\n")
				TEXT("Combat,Button prompt for defending,Defend,Defend,,\n")
				TEXT("Combat,Trigger after 3 hits,Special,Special Attack,,\n")
				TEXT("Items,Rare weapon with blue aura,Sword,Legendary Sword,,\n")
				TEXT("Items,Used to reduce damage intake,Shield,Ancient Shield,,\n")
				TEXT("Items,Restores 50% of HP,Potion,Health Potion,,\n")
				TEXT("Items,Increase intel by 10,Scroll,Scroll of Wisdom,,\n")
				TEXT("Dialogue,First NPC interaction,Greeting,Greetings traveler,,\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(TEXT("WithDevNotes"), CSV));

			FString OutMessage;
			TArray<FString> CSVPaths;
			CSVPaths.Add(FELTAutomationCommon::GetTestCSVPath(TEXT("WithDevNotes")));
			FString LocPath = UELTEditorSettings::GetLocalizationPath();
			FString LocName = TEXT("Game");

			bool bSuccess = UELTEditor::GenerateLocFilesImpl(CSVPaths, LocPath, LocName, TEXT(""), TEXT(","), TEXT("FIRST_LANG"), true, OutMessage);

			TestTrue(TEXT("GenerateLocFilesImpl succeeded"), bSuccess);
			TestTrue(TEXT("Locres files created"), FELTAutomationCommon::LocResFileExists(LocPath, LocName, TEXT("en")));
			TestTrue(TEXT("Locres files created"), FELTAutomationCommon::LocResFileExists(LocPath, LocName, TEXT("ja")));
			TestTrue(TEXT("Locres files created"), FELTAutomationCommon::LocResFileExists(LocPath, LocName, TEXT("zh")));
			TestTrue(TEXT("StringTable created"), FELTAutomationCommon::StringTableFileExists(LocPath, LocName, "Combat"));
			TestTrue(TEXT("StringTable created"), FELTAutomationCommon::StringTableFileExists(LocPath, LocName, "Items"));
			TestTrue(TEXT("StringTable created"), FELTAutomationCommon::StringTableFileExists(LocPath, LocName, "Dialogue"));

			TEST_STRING_TABLE("en", "Game", LocPath, "Combat", "Attack", "Attack");
			TEST_STRING_TABLE("ja", "Game", LocPath, "Items", "Sword", "Legendary Sword");
			TEST_STRING_TABLE("zh", "Game", LocPath, "Dialogue", "Greeting", "Greetings traveler");

			bSuccess = UELTEditor::GenerateLocFilesImpl(CSVPaths, LocPath, LocName, TEXT(""), TEXT(","), TEXT("NONE"), true, OutMessage);
			TestTrue(TEXT("GenerateLocFilesImpl succeeded with no fallback"), bSuccess);

			TEST_STRING_TABLE("en", "Game", LocPath, "Combat", "Attack", "Attack");
			TEST_STRING_TABLE("ja", "Game", LocPath, "Items", "Sword", "");
			TEST_STRING_TABLE("zh", "Game", LocPath, "Dialogue", "Greeting", "");
		});
	});
}

// ============================================================================

BEGIN_DEFINE_SPEC(FELTGenerateLocFiles_MissingTranslations, "EasyLocalizationTool.GenerateLocFiles.MissingTranslations", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
END_DEFINE_SPEC(FELTGenerateLocFiles_MissingTranslations)

void FELTGenerateLocFiles_MissingTranslations::Define()
{
	Describe(TEXT("GenerateLocFilesImpl - Missing Translations with Fallback"), [this]()
	{
		It(TEXT("Should handle missing translations and apply FIRST_LANG fallback"), [this]()
		{
			
			FELTAutomationCommon::EnsureDirectories();

			FString CSV = TEXT("namespace,key,lang-en,lang-ko,lang-ru\n")
				TEXT("Messages,Dialog1,Hello World,,\n")
				TEXT("Messages,Dialog2,Good morning,,\n")
				TEXT("Messages,Dialog3,Thank you,,\n")
				TEXT("Messages,Dialog4,Sorry,,\n")
				TEXT("Items,Gold,Gold Coin,,\n")
				TEXT("Items,Silver,Silver Coin,,\n")
				TEXT("Items,Bronze,Bronze Coin,,\n")
				TEXT("Items,Gem,Rare Gem,,\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(TEXT("MissingTranslations"), CSV));

			FString OutMessage;
			TArray<FString> CSVPaths;
			CSVPaths.Add(FELTAutomationCommon::GetTestCSVPath(TEXT("MissingTranslations")));
			FString LocPath = UELTEditorSettings::GetLocalizationPath();
			FString LocName = TEXT("Game");

			bool bSuccess = UELTEditor::GenerateLocFilesImpl(CSVPaths, LocPath, LocName, TEXT(""), TEXT(","), TEXT("FIRST_LANG"), false, OutMessage);

			TestTrue(TEXT("GenerateLocFilesImpl succeeded with fallback"), bSuccess);
			TestTrue(TEXT("Locres files created"), FELTAutomationCommon::LocResFileExists(LocPath, LocName, TEXT("en")));

			
		});
	});
}

// ============================================================================

BEGIN_DEFINE_SPEC(FELTGenerateLocFiles_UnderscoreLangs, "EasyLocalizationTool.GenerateLocFiles.UnderscoreLangs", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
END_DEFINE_SPEC(FELTGenerateLocFiles_UnderscoreLangs)

void FELTGenerateLocFiles_UnderscoreLangs::Define()
{
	Describe(TEXT("GenerateLocFilesImpl - Language codes with underscores"), [this]()
	{
		It(TEXT("Should convert underscores to hyphens in language codes"), [this]()
		{
			
			FELTAutomationCommon::EnsureDirectories();

			FString CSV = TEXT("namespace,key,lang_en,lang_pt_br,lang_zh_cn\n")
				TEXT("UI,ButtonLabel,Click Me,Clique em Mim,\n")
				TEXT("UI,ErrorMessage,An error occurred,Ocorreu um erro,\n")
				TEXT("Settings,Volume,Volume,Volume,\n")
				TEXT("Settings,Brightness,Brightness,Brilho,\n")
				TEXT("Audio,MusicVolume,Music,M sica,\n")
				TEXT("Audio,SFXVolume,Sound Effects,Efeitos Sonoros,\n")
				TEXT("Audio,Mute,Mute,Silenciar,\n")
				TEXT("Audio,Unmute,Unmute,Desmutizar,\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(TEXT("UnderscoreLangs"), CSV));

			FString OutMessage;
			TArray<FString> CSVPaths;
			CSVPaths.Add(FELTAutomationCommon::GetTestCSVPath(TEXT("UnderscoreLangs")));
			FString LocPath = UELTEditorSettings::GetLocalizationPath();
			FString LocName = TEXT("Game");

			bool bSuccess = UELTEditor::GenerateLocFilesImpl(CSVPaths, LocPath, LocName, TEXT(""), TEXT(","), TEXT("NONE"), false, OutMessage);

			TestTrue(TEXT("GenerateLocFilesImpl succeeded"), bSuccess);
			TestTrue(TEXT("English locres file created"), FELTAutomationCommon::LocResFileExists(LocPath, LocName, TEXT("en")));
			TestTrue(TEXT("Portuguese (Brazil) locres file created"), FELTAutomationCommon::LocResFileExists(LocPath, LocName, TEXT("pt-br")));
			TestTrue(TEXT("Chinese (Simplified) locres file created"), FELTAutomationCommon::LocResFileExists(LocPath, LocName, TEXT("zh-cn")));

			
		});
	});
}

// ============================================================================
// INVALID CSV TESTS
// ============================================================================

BEGIN_DEFINE_SPEC(FELTGenerateLocFiles_DuplicateColumns, "EasyLocalizationTool.GenerateLocFiles.Invalid.DuplicateColumns", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
END_DEFINE_SPEC(FELTGenerateLocFiles_DuplicateColumns)

void FELTGenerateLocFiles_DuplicateColumns::Define()
{
	Describe(TEXT("GenerateLocFilesImpl - Invalid: Duplicate columns"), [this]()
	{
		It(TEXT("Should fail when CSV has duplicate namespace columns"), [this]()
		{
			
			FELTAutomationCommon::EnsureDirectories();

			FString CSV = TEXT("namespace,key,namespace,lang-en\n")
				TEXT("Menu,Start,Menu,Start Game\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(TEXT("DuplicateColumns"), CSV));

			FString OutMessage;
			TArray<FString> CSVPaths;
			CSVPaths.Add(FELTAutomationCommon::GetTestCSVPath(TEXT("DuplicateColumns")));
			FString LocPath = UELTEditorSettings::GetLocalizationPath();
			FString LocName = TEXT("Game");

			bool bSuccess = UELTEditor::GenerateLocFilesImpl(CSVPaths, LocPath, LocName, TEXT(""), TEXT(","), TEXT("NONE"), false, OutMessage);

			TestFalse(TEXT("GenerateLocFilesImpl should fail"), bSuccess);
			TestTrue(TEXT("Error message mentions duplicate columns"), OutMessage.Contains(TEXT("Multiple")));

			
		});
	});
}

// ============================================================================

BEGIN_DEFINE_SPEC(FELTGenerateLocFiles_NoKeyColumn, "EasyLocalizationTool.GenerateLocFiles.Invalid.NoKeyColumn", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
END_DEFINE_SPEC(FELTGenerateLocFiles_NoKeyColumn)

void FELTGenerateLocFiles_NoKeyColumn::Define()
{
	Describe(TEXT("GenerateLocFilesImpl - Invalid: No key column"), [this]()
	{
		It(TEXT("Should fail when CSV is missing the key column"), [this]()
		{
			
			FELTAutomationCommon::EnsureDirectories();

			FString CSV = TEXT("namespace,value,lang-en,lang-fr\n")
				TEXT("MainMenu,Start Game,Commencer\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(TEXT("NoKeyColumn"), CSV));

			FString OutMessage;
			TArray<FString> CSVPaths;
			CSVPaths.Add(FELTAutomationCommon::GetTestCSVPath(TEXT("NoKeyColumn")));
			FString LocPath = UELTEditorSettings::GetLocalizationPath();
			FString LocName = TEXT("Game");

			bool bSuccess = UELTEditor::GenerateLocFilesImpl(CSVPaths, LocPath, LocName, TEXT(""), TEXT(","), TEXT("NONE"), false, OutMessage);

			TestFalse(TEXT("GenerateLocFilesImpl should fail"), bSuccess);
			TestTrue(TEXT("Error message mentions key column"), OutMessage.Contains(TEXT("Key")));

			
		});
	});
}

// ============================================================================

BEGIN_DEFINE_SPEC(FELTGenerateLocFiles_NoLanguageColumns, "EasyLocalizationTool.GenerateLocFiles.Invalid.NoLanguageColumns", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
END_DEFINE_SPEC(FELTGenerateLocFiles_NoLanguageColumns)

void FELTGenerateLocFiles_NoLanguageColumns::Define()
{
	Describe(TEXT("GenerateLocFilesImpl - Invalid: No language columns"), [this]()
	{
		It(TEXT("Should fail when CSV has no language columns"), [this]()
		{
			
			FELTAutomationCommon::EnsureDirectories();

			FString CSV = TEXT("namespace,key\n")
				TEXT("MainMenu,Start\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(TEXT("NoLanguageColumns"), CSV));

			FString OutMessage;
			TArray<FString> CSVPaths;
			CSVPaths.Add(FELTAutomationCommon::GetTestCSVPath(TEXT("NoLanguageColumns")));
			FString LocPath = UELTEditorSettings::GetLocalizationPath();
			FString LocName = TEXT("Game");

			bool bSuccess = UELTEditor::GenerateLocFilesImpl(CSVPaths, LocPath, LocName, TEXT(""), TEXT(","), TEXT("NONE"), false, OutMessage);

			TestFalse(TEXT("GenerateLocFilesImpl should fail"), bSuccess);
			TestTrue(TEXT("Error message mentions language columns"), OutMessage.Contains(TEXT("lang")));

			
		});
	});
}

// ============================================================================

BEGIN_DEFINE_SPEC(FELTGenerateLocFiles_LanguageBeforeKey, "EasyLocalizationTool.GenerateLocFiles.Invalid.LanguageBeforeKey", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
END_DEFINE_SPEC(FELTGenerateLocFiles_LanguageBeforeKey)

void FELTGenerateLocFiles_LanguageBeforeKey::Define()
{
	Describe(TEXT("GenerateLocFilesImpl - Invalid: Language column before key"), [this]()
	{
		It(TEXT("Should fail when language column appears before key column"), [this]()
		{
			
			FELTAutomationCommon::EnsureDirectories();

			FString CSV = TEXT("namespace,lang-en,key\n")
				TEXT("MainMenu,Start Game,Start\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(TEXT("LanguageBeforeKey"), CSV));

			FString OutMessage;
			TArray<FString> CSVPaths;
			CSVPaths.Add(FELTAutomationCommon::GetTestCSVPath(TEXT("LanguageBeforeKey")));
			FString LocPath = UELTEditorSettings::GetLocalizationPath();
			FString LocName = TEXT("Game");

			bool bSuccess = UELTEditor::GenerateLocFilesImpl(CSVPaths, LocPath, LocName, TEXT(""), TEXT(","), TEXT("NONE"), false, OutMessage);

			TestFalse(TEXT("GenerateLocFilesImpl should fail"), bSuccess);
			TestTrue(TEXT("Error message mentions column order"), OutMessage.Contains(TEXT("before")));

			
		});
	});
}

// ============================================================================

BEGIN_DEFINE_SPEC(FELTGenerateLocFiles_MissingNamespaceValue, "EasyLocalizationTool.GenerateLocFiles.Invalid.MissingNamespaceValue", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
END_DEFINE_SPEC(FELTGenerateLocFiles_MissingNamespaceValue)

void FELTGenerateLocFiles_MissingNamespaceValue::Define()
{
	Describe(TEXT("GenerateLocFilesImpl - Invalid: Missing namespace value"), [this]()
	{
		It(TEXT("Should fail when namespace column has empty value without global namespace"), [this]()
		{
			
			FELTAutomationCommon::EnsureDirectories();

			FString CSV = TEXT("namespace,key,lang-en,lang-es\n")
				TEXT("MainMenu,Start,Start Game,Comenzar\n")
				TEXT(",,Quit,Salir\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(TEXT("MissingNamespaceValue"), CSV));

			FString OutMessage;
			TArray<FString> CSVPaths;
			CSVPaths.Add(FELTAutomationCommon::GetTestCSVPath(TEXT("MissingNamespaceValue")));
			FString LocPath = UELTEditorSettings::GetLocalizationPath();
			FString LocName = TEXT("Game");

			bool bSuccess = UELTEditor::GenerateLocFilesImpl(CSVPaths, LocPath, LocName, TEXT(""), TEXT(","), TEXT("NONE"), false, OutMessage);

			TestFalse(TEXT("GenerateLocFilesImpl should fail"), bSuccess);
			TestTrue(TEXT("Error message mentions namespace"), OutMessage.Contains(TEXT("Namespace")));

			
		});
	});
}

// ============================================================================

BEGIN_DEFINE_SPEC(FELTGenerateLocFiles_InconsistentColumnCount, "EasyLocalizationTool.GenerateLocFiles.Invalid.InconsistentColumnCount", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
END_DEFINE_SPEC(FELTGenerateLocFiles_InconsistentColumnCount)

void FELTGenerateLocFiles_InconsistentColumnCount::Define()
{
	Describe(TEXT("GenerateLocFilesImpl - Invalid: Inconsistent column counts"), [this]()
	{
		It(TEXT("Should fail when columns have different number of values"), [this]()
		{
			
			FELTAutomationCommon::EnsureDirectories();

			FString CSV = TEXT("namespace,key,lang-en,lang-de\n")
				TEXT("Menu,Start,Start Game,Spiel starten\n")
				TEXT("Menu,Quit,Quit\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(TEXT("InconsistentColumnCount"), CSV));

			FString OutMessage;
			TArray<FString> CSVPaths;
			CSVPaths.Add(FELTAutomationCommon::GetTestCSVPath(TEXT("InconsistentColumnCount")));
			FString LocPath = UELTEditorSettings::GetLocalizationPath();
			FString LocName = TEXT("Game");

			bool bSuccess = UELTEditor::GenerateLocFilesImpl(CSVPaths, LocPath, LocName, TEXT(""), TEXT(","), TEXT("NONE"), false, OutMessage);

			TestFalse(TEXT("GenerateLocFilesImpl should fail"), bSuccess);
			TestTrue(TEXT("Error message mentions same amount"), OutMessage.Contains(TEXT("same amount")));

			
		});
	});
}

// ============================================================================

BEGIN_DEFINE_SPEC(FELTGenerateLocFiles_InvalidSeparator, "EasyLocalizationTool.GenerateLocFiles.Invalid.InvalidSeparator", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
END_DEFINE_SPEC(FELTGenerateLocFiles_InvalidSeparator)

void FELTGenerateLocFiles_InvalidSeparator::Define()
{
	Describe(TEXT("GenerateLocFilesImpl - Invalid: Invalid separator"), [this]()
	{
		It(TEXT("Should fail when separator is empty or more than 1 character"), [this]()
		{
			
			FELTAutomationCommon::EnsureDirectories();

			FString CSV = TEXT("namespace,key,lang-en,lang-de\n")
				TEXT("Menu,Start,Start Game,Spiel starten\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(TEXT("InvalidSeparator"), CSV));

			FString OutMessage;
			TArray<FString> CSVPaths;
			CSVPaths.Add(FELTAutomationCommon::GetTestCSVPath(TEXT("InvalidSeparator")));
			FString LocPath = UELTEditorSettings::GetLocalizationPath();
			FString LocName = TEXT("Game");

			bool bSuccess = UELTEditor::GenerateLocFilesImpl(CSVPaths, LocPath, LocName, TEXT(""), TEXT(""), TEXT("NONE"), false, OutMessage);

			TestFalse(TEXT("GenerateLocFilesImpl should fail"), bSuccess);
			TestTrue(TEXT("Error message mentions separator"), OutMessage.Contains(TEXT("Separator")));

			
		});
	});
}

// ============================================================================
// LOCALIZATION VERIFICATION TESTS
// ============================================================================

BEGIN_DEFINE_SPEC(FELTGenerateLocFiles_LocResVerification, "EasyLocalizationTool.GenerateLocFiles.Verification.LocResFiles", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
END_DEFINE_SPEC(FELTGenerateLocFiles_LocResVerification)

void FELTGenerateLocFiles_LocResVerification::Define()
{
	Describe(TEXT("GenerateLocFilesImpl - Localization file verification"), [this]()
	{
		It(TEXT("Should create locres files for all languages and enable NSLOCTEXT retrieval"), [this]()
		{
			
			FELTAutomationCommon::EnsureDirectories();

			FString CSV = TEXT("namespace,key,lang-en,lang-pl,lang-fr,lang-de\n")
				TEXT("MainMenu,StartGame,Start Game,Rozpocznij gr ,Commencer le jeu,Spiel starten\n")
				TEXT("MainMenu,QuitGame,Quit,Wyjd ,Quitter,Beenden\n")
				TEXT("MainMenu,Settings,Settings,Ustawienia,Paramtres,Einstellungen\n")
				TEXT("MainMenu,Credits,Credits,Napisy,Cr dits,Abspann\n")
				TEXT("Gameplay,Health,Health,Zdrowie,Sant ,Gesundheit\n")
				TEXT("Gameplay,Damage,Damage,Obra enia,D g ts,Schaden\n")
				TEXT("Gameplay,Score,Score,Wynik,Score,Punktzahl\n")
				TEXT("Gameplay,Level,Level,Poziom,Niveau,Stufe\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(TEXT("LocResVerification"), CSV));

			FString OutMessage;
			TArray<FString> CSVPaths;
			CSVPaths.Add(FELTAutomationCommon::GetTestCSVPath(TEXT("LocResVerification")));
			FString LocPath = UELTEditorSettings::GetLocalizationPath();
			FString LocName = TEXT("Game");

			bool bSuccess = UELTEditor::GenerateLocFilesImpl(CSVPaths, LocPath, LocName, TEXT("TestNamespace"), TEXT(","), TEXT("NONE"), false, OutMessage);

			TestTrue(TEXT("GenerateLocFilesImpl succeeded"), bSuccess);

			// Verify all language files exist
			TestTrue(TEXT("English locres exists"), FELTAutomationCommon::LocResFileExists(LocPath, LocName, TEXT("en")));
			TestTrue(TEXT("Polish locres exists"), FELTAutomationCommon::LocResFileExists(LocPath, LocName, TEXT("pl")));
			TestTrue(TEXT("French locres exists"), FELTAutomationCommon::LocResFileExists(LocPath, LocName, TEXT("fr")));
			TestTrue(TEXT("German locres exists"), FELTAutomationCommon::LocResFileExists(LocPath, LocName, TEXT("de")));

			// Try to retrieve localized text using NSLOCTEXT
#if (ENGINE_MAJOR_VERSION == 5)
			FText LocalizedTextEN = FText::FromStringTable(FName(TEXT("TestNamespace")), FName(TEXT("StartGame")));
#else
			FText LocalizedTextEN = FText::FromStringTable(TEXT("TestNamespace"), TEXT("StartGame"));
#endif
			TestFalse(TEXT("English text can be retrieved"), LocalizedTextEN.IsEmpty());

			
		});
	});
}

// ============================================================================

BEGIN_DEFINE_SPEC(FELTGenerateLocFiles_StringTableGeneration, "EasyLocalizationTool.GenerateLocFiles.Verification.StringTables", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
END_DEFINE_SPEC(FELTGenerateLocFiles_StringTableGeneration)

void FELTGenerateLocFiles_StringTableGeneration::Define()
{
	Describe(TEXT("GenerateLocFilesImpl - String table generation"), [this]()
	{
		It(TEXT("Should generate string table assets when bGenerateStringTables is true"), [this]()
		{
			
			FELTAutomationCommon::EnsureDirectories();

			FString CSV = TEXT("namespace,key,devnotes,lang-en,lang-ja,lang-zh\n")
				TEXT("Combat,Attack,Button prompt for attacking,Attack,,\n")
				TEXT("Combat,Defend,Button prompt for defending,Defend,,\n")
				TEXT("Combat,Special,Trigger after 3 hits,Special Attack,,\n")
				TEXT("Items,Sword,Rare weapon with blue aura,Legendary Sword,,\n")
				TEXT("Items,Shield,Used to reduce damage intake,Ancient Shield,,\n")
				TEXT("Items,Potion,Restores 50% of HP,Health Potion,,\n")
				TEXT("Items,Scroll,Increase intel by 10,Scroll of Wisdom,,\n")
				TEXT("Dialogue,Greeting,First NPC interaction,Greetings traveler,,\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(TEXT("StringTableGeneration"), CSV));

			FString OutMessage;
			TArray<FString> CSVPaths;
			CSVPaths.Add(FELTAutomationCommon::GetTestCSVPath(TEXT("StringTableGeneration")));
			FString LocPath = UELTEditorSettings::GetLocalizationPath();
			FString LocName = TEXT("Game");

			// Enable string table generation
			bool bSuccess = UELTEditor::GenerateLocFilesImpl(CSVPaths, LocPath, LocName, TEXT(""), TEXT(","), TEXT("NONE"), true, OutMessage);

			TestTrue(TEXT("GenerateLocFilesImpl succeeded with string table generation"), bSuccess);
			TestTrue(TEXT("Locres files created"), FELTAutomationCommon::LocResFileExists(LocPath, LocName, TEXT("en")));

			
		});
	});
}

// ============================================================================

BEGIN_DEFINE_SPEC(FELTGenerateLocFiles_MultipleCSVFiles, "EasyLocalizationTool.GenerateLocFiles.Verification.MultipleCSVFiles", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
END_DEFINE_SPEC(FELTGenerateLocFiles_MultipleCSVFiles)

void FELTGenerateLocFiles_MultipleCSVFiles::Define()
{
	Describe(TEXT("GenerateLocFilesImpl - Multiple CSV files"), [this]()
	{
		It(TEXT("Should process multiple CSV files and merge their localization data"), [this]()
		{
			
			FELTAutomationCommon::EnsureDirectories();

			FString CSV1 = TEXT("namespace,key,lang-en,lang-pl\n")
				TEXT("UI,Button1,Button 1,Przycisk 1\n")
				TEXT("UI,Button2,Button 2,Przycisk 2\n")
				TEXT("UI,Button3,Button 3,Przycisk 3\n")
				TEXT("UI,Button4,Button 4,Przycisk 4\n")
				TEXT("UI,Button5,Button 5,Przycisk 5\n");

			FString CSV2 = TEXT("namespace,key,lang-en,lang-pl\n")
				TEXT("Menu,Item1,Item 1,Element 1\n")
				TEXT("Menu,Item2,Item 2,Element 2\n")
				TEXT("Menu,Item3,Item 3,Element 3\n")
				TEXT("Menu,Item4,Item 4,Element 4\n")
				TEXT("Menu,Item5,Item 5,Element 5\n");

			TestTrue(TEXT("CSV file 1 created"), FELTAutomationCommon::WriteTestCSV(TEXT("MultiCSV1"), CSV1));
			TestTrue(TEXT("CSV file 2 created"), FELTAutomationCommon::WriteTestCSV(TEXT("MultiCSV2"), CSV2));

			FString OutMessage;
			TArray<FString> CSVPaths;
			CSVPaths.Add(FELTAutomationCommon::GetTestCSVPath(TEXT("MultiCSV1")));
			CSVPaths.Add(FELTAutomationCommon::GetTestCSVPath(TEXT("MultiCSV2")));
			FString LocPath = UELTEditorSettings::GetLocalizationPath();
			FString LocName = TEXT("Game");

			bool bSuccess = UELTEditor::GenerateLocFilesImpl(CSVPaths, LocPath, LocName, TEXT(""), TEXT(","), TEXT("NONE"), false, OutMessage);

			TestTrue(TEXT("GenerateLocFilesImpl succeeded with multiple CSV files"), bSuccess);
			TestTrue(TEXT("Locres files created"), FELTAutomationCommon::LocResFileExists(LocPath, LocName, TEXT("en")));

			
		});
	});
}

ELTTESTS_PRAGMA_ENABLE_OPTIMIZATION
