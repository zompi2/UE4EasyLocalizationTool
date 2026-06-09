// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#if WITH_EDITOR

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

#if (ENGINE_MAJOR_VERSION == 5)
#define SWITCH_LANG(_Lang) { \
	bLangChanged = false; \
	FTextLocalizationManager::Get().DisableGameLocalizationPreview(); \
	while (!bLangChanged) { FPlatformProcess::Sleep(0.1f); } \
	bLangChanged = false; \
	FTextLocalizationManager::Get().EnableGameLocalizationPreview(TEXT(_Lang)); \
	while (!bLangChanged) { FPlatformProcess::Sleep(0.1f); } \
}
#else
#define SWITCH_LANG(_Lang) { \
	FTextLocalizationManager::Get().DisableGameLocalizationPreview(); \
	FTextLocalizationManager::Get().EnableGameLocalizationPreview(TEXT(_Lang)); \
}
#endif

#define TEST_TRANSLATION(_Lang, _Namespace, _Key, _Expected) { \
	SWITCH_LANG(_Lang) \
	FString Result = NSLOCTEXT(_Namespace, _Key, _Key).ToString(); \
	TestTrue( \
		FString::Printf( \
			TEXT("Test lang %s of namespace: %s key: %s expect: %s result: %s"), \
			TEXT(_Lang), TEXT(_Namespace), TEXT(_Key), TEXT(_Expected), *Result \
		), \
		Result.Equals(TEXT(_Expected)) \
	); \
}

#define GET_STRING_TABLE_NAME(_Namespace) const FString StringTableName = UELTEditor::GetStringTableName(LocName, _Namespace); \
const FString StringTableID = FPackageName::FilenameToLongPackageName(LocPath / StringTableName + TEXT(".uasset")) + TEXT(".") + StringTableName;

#define TEST_STRING_TABLE(_Lang, _Namespace, _Key, _Expected) { \
	GET_STRING_TABLE_NAME(_Namespace) \
	SWITCH_LANG(_Lang) \
	FString Result = TEXT(""); \
	bool bSTDone = false; \
	AsyncTask(ENamedThreads::GameThread, [this, &StringTableID, &Result, &bSTDone]() \
	{ \
		FText LocalizedText = FText::FromStringTable(FName(StringTableID), TEXT(_Key), EStringTableLoadingPolicy::FindOrLoad); \
		Result = LocalizedText.ToString(); \
		bSTDone = true; \
	}); \
	while (bSTDone == false) { FPlatformProcess::Sleep(0.1f); } \
	TestTrue( \
		FString::Printf( \
			TEXT("Test String Table: %s lang %s key: %s expect: %s result: %s"), \
			*StringTableID, TEXT(_Lang), TEXT(_Key), TEXT(_Expected), *Result \
		), \
		Result.Equals(TEXT(_Expected)) \
	); \
}

#if ((ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION >= 8) && WITH_EDITORONLY_DATA)	
#define TEST_STRING_TABLE_DEVNOTE(_Namespace, _Key, _Expected) { \
	GET_STRING_TABLE_NAME(_Namespace) \
	bool bResult = false; \
	FString DevNote = TEXT(""); \
	FStringTableConstPtr StringTable = FStringTableRegistry::Get().FindStringTable(*StringTableID); \
	if (StringTable) \
	{ \
		FStringTableEntryConstPtr Entry = StringTable->FindEntry(FTextKey(_Key)); \
		if (Entry) \
		{ \
			DevNote = Entry->GetDevNotes(); \
			bResult = DevNote.Equals(TEXT(_Expected)); \
		} \
	} \
	TestTrue( \
		FString::Printf( \
			TEXT("Test String Table DevNote: %s key: %s expect: %s result: %s"), \
			*StringTableID, TEXT(_Key), TEXT(_Expected), *DevNote \
		), \
		bResult \
	); \
}
#endif

#define TEST_LOCFILE_EXISTS(_Lang) { \
	TestTrue(TEXT(#_Lang" exists"), FELTAutomationCommon::LocResFileExists(LocPath, LocName, _Lang)); \
}

#define TEST_STRING_TABLE_EXISTS(_Namespace) { \
	TestTrue(TEXT(#_Namespace" string table exists"), FELTAutomationCommon::StringTableFileExists(LocPath, LocName, TEXT(_Namespace))); \
}

#define IMPORT_CSV_ASYNC(_Namespace, _Separator, _Fallback, _GenStringTable, _ExpectTrue, _ExpectedError) {\
	bool bSuccess = false; \
	bool bDone = false; \
	FString OutMessage; \
	AsyncTask(ENamedThreads::GameThread, [this, &bSuccess, &bDone, &OutMessage]() \
	{ \
		bSuccess = UELTEditor::GenerateLocFilesImpl( \
			FELTAutomationCommon::GetTestCSVPath(CSVName), \
			LocPath, \
			LocName, \
			TEXT(_Namespace), \
			TEXT(_Separator), \
			TEXT(_Fallback), \
			_GenStringTable, \
			OutMessage); \
		bDone = true; \
	}); \
	while (bDone == false) { FPlatformProcess::Sleep(0.1f); } \
	if (_ExpectTrue) \
	{ \
		TestTrue(TEXT("GenerateLocFilesImpl succeeded"), bSuccess); \
	} \
	else \
	{ \
		TestFalse(TEXT("GenerateLocFilesImpl failed (and it's good)"), bSuccess); \
		TestTrue(FString::Printf(TEXT("Error expected = %s Result = %s"), TEXT(_ExpectedError), *OutMessage), OutMessage.Contains(TEXT(_ExpectedError)));\
	} \
}

BEGIN_DEFINE_SPEC(FELTTests, "EasyLocalizationTool.Tests", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
FString LocName = TEXT("Game");
FString LocPath = UELTEditorSettings::GetLocalizationPath();
FString CSVName = TEXT("CSVTest");
#if (ENGINE_MAJOR_VERSION == 5)
bool bLangChanged = false;
FDelegateHandle OnLangChangedHandle;
#endif
END_DEFINE_SPEC(FELTTests)

void FELTTests::Define()
{
	Describe(TEXT("ELT - PRE PASS"), [this]()
	{
		It(TEXT("Just Runs a Pre Run"), [this]()
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
				TEXT(""),
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
		});
	});

	Describe(TEXT("ELT - Import Tests"), [this]()
	{
		BeforeEach([this]()
		{
			FELTAutomationCommon::MakeTestDirectory();
#if (ENGINE_MAJOR_VERSION == 5)
			OnLangChangedHandle = FTextLocalizationManager::Get().OnTextRevisionChangedEvent.AddLambda([this]()
			{
				bLangChanged = true;
			});
#endif
		});

		AfterEach([this]()
		{
			FELTAutomationCommon::CleanTestDirectory();
#if (ENGINE_MAJOR_VERSION == 5)
			FTextLocalizationManager::Get().OnTextRevisionChangedEvent.Remove(OnLangChangedHandle);
#endif
		});

		// ====================== 1. TESTS: VALID CSV WITH NAMESPACE ======================
		LatentIt(TEXT("VALID CSV WITH NAMESPACE"), EAsyncExecution::ThreadPool, [this](const FDoneDelegate TestDone)
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

			IMPORT_CSV_ASYNC("GlobalNamespace", ",", "NONE", false, true, "");

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

			TestDone.Execute();
		});

		// ====================== 2. TESTS: VALID CSV WITHOUT NAMESPACE ======================
		LatentIt(TEXT("VALID CSV WITHOUT NAMESPACE"), EAsyncExecution::ThreadPool, [this](const FDoneDelegate TestDone)
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

			IMPORT_CSV_ASYNC("GlobalNamespace", ",", "NONE", false, true, "");

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

			TestDone.Execute();
		});

		// ====================== 3. TESTS: VALID CSV WITH NAMESPACE AND DEVNOTES AND STRING TABLE ======================
		LatentIt(TEXT("VALID CSV WITH NAMESPACE AND DEVNOTES AND STRING TABLE"), EAsyncExecution::ThreadPool, [this](const FDoneDelegate TestDone)
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

				IMPORT_CSV_ASYNC("GlobalNamespace", ",", "NONE", true, true, "");

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

#if ((ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION >= 8) && WITH_EDITORONLY_DATA)	
				TEST_STRING_TABLE_DEVNOTE("MainMenu", "StartGame", "DevNote_1");
				TEST_STRING_TABLE_DEVNOTE("MainMenu", "Settings", "DevNote_3");
				TEST_STRING_TABLE_DEVNOTE("Gameplay", "Score", "DevNote_7");
#endif

				TestDone.Execute();
		});

		// ====================== 4. TESTS: VALID CSV WITH NAMESPACE AND DEVNOTES AND STRING TABLES IN OTHER ORDER ======================
		LatentIt(TEXT("VALID CSV WITH NAMESPACE AND DEVNOTES AND STRING TABLES IN OTHER ORDER"), EAsyncExecution::ThreadPool, [this](const FDoneDelegate TestDone)
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

				IMPORT_CSV_ASYNC("GlobalNamespace", ",", "NONE", true, true, "");

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

#if ((ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION >= 8) && WITH_EDITORONLY_DATA)	
				TEST_STRING_TABLE_DEVNOTE("MainMenu", "StartGame", "DevNote_1");
				TEST_STRING_TABLE_DEVNOTE("MainMenu", "Settings", "DevNote_3");
				TEST_STRING_TABLE_DEVNOTE("Gameplay", "Score", "DevNote_7");
#endif
				TestDone.Execute();
		});

		// ====================== 4-B. TESTS: VALID CSV WITH NAMESPACE AND DEVNOTES AND STRING TABLES WITH KEYS IN OTHER ORDER ======================
		LatentIt(TEXT("VALID CSV WITH NAMESPACE AND DEVNOTES AND STRING TABLES WITH KEYS IN OTHER ORDER"), EAsyncExecution::ThreadPool, [this](const FDoneDelegate TestDone)
		{
			const FString CSV = TEXT("key,namespace,devnotes,lang-en,lang-pl,lang-fr,lang-de\n")
				TEXT("StartGame,MainMenu,DevNote_1,Start Game,Rozpocznij gre,Commencer le jeu,Spiel starten\n")
				TEXT("QuitGame,MainMenu,DevNote_2,Quit,Wyjd,Quitter,Beenden\n")
				TEXT("Settings,MainMenu,DevNote_3,Settings,Ustawienia,Paramtres,Einstellungen\n")
				TEXT("Credits,MainMenu,DevNote_4,Credits,Napisy,Cr dits,Abspann\n")
				TEXT("Health,Gameplay,DevNote_5,Health,Zdrowie,Sant,Gesundheit\n")
				TEXT("Damage,Gameplay,DevNote_6,Damage,Obra enia,D g ts,Schaden\n")
				TEXT("Score,Gameplay,DevNote_7,Score,Wynik,Score,Punktzahl\n")
				TEXT("Level,Gameplay,DevNote_8,Level,Poziom,Niveau,Stufe\n");

				TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(CSVName, CSV));

				IMPORT_CSV_ASYNC("GlobalNamespace", ",", "NONE", true, true, "");

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

#if ((ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION >= 8) && WITH_EDITORONLY_DATA)	
				TEST_STRING_TABLE_DEVNOTE("MainMenu", "StartGame", "DevNote_1");
				TEST_STRING_TABLE_DEVNOTE("MainMenu", "Settings", "DevNote_3");
				TEST_STRING_TABLE_DEVNOTE("Gameplay", "Score", "DevNote_7");
#endif
				TestDone.Execute();
		});

		// ====================== 4-c. TESTS: VALID CSV WITH NAMESPACE AND DEVNOTES AND STRING TABLES IN OTHER ORDER (DevNotesAtEnd) ======================
		LatentIt(TEXT("VALID CSV WITH NAMESPACE AND DEVNOTES AND STRING TABLES IN OTHER ORDER (DevNotesAtEnd)"), EAsyncExecution::ThreadPool, [this](const FDoneDelegate TestDone)
		{
			const FString CSV = TEXT("namespace,key,devnotes,lang-en,lang-pl,lang-fr,lang-de\n")
				TEXT("MainMenu,StartGame,DevNote_1,Start Game,Rozpocznij gre,Commencer le jeu,Spiel starten\n")
				TEXT("MainMenu,QuitGame,DevNote_2,Quit,Wyjd ,Quitter,Beenden\n")
				TEXT("MainMenu,Settings,DevNote_3,Settings,Ustawienia,Paramtres,Einstellungen\n")
				TEXT("Credits,Credits,DevNote_4,Napisy,Cr dits,Abspann,\n")
				TEXT("Gameplay,Health,DevNote_5,Health,Zdrowie,Sant,Gesundheit\n")
				TEXT("Gameplay,Damage,DevNote_6,Damage,Obra enia,D g ts,Schaden\n")
				TEXT("Gameplay,Score,DevNote_7,Score,Wynik,Score,Punktzahl\n")
				TEXT("Gameplay,Level,DevNote_8,Level,Poziom,Niveau,Stufe\n");

				TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(CSVName, CSV));

				IMPORT_CSV_ASYNC("GlobalNamespace", ",", "NONE", true, true, "");

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

#if ((ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION >= 8) && WITH_EDITORONLY_DATA)	
				TEST_STRING_TABLE_DEVNOTE("MainMenu", "StartGame", "DevNote_1");
				TEST_STRING_TABLE_DEVNOTE("MainMenu", "Settings", "DevNote_3");
				TEST_STRING_TABLE_DEVNOTE("Gameplay", "Score", "DevNote_7");
#endif
				TestDone.Execute();
		});

		// ====================== 6. TESTS: INVALID CSV WITHOUT NAMESPACE AND NO GLOBAL NAMESPACE ======================
		LatentIt(TEXT("INVALID CSV WITHOUT NAMESPACE AND NO GLOBAL NAMESPACE"), EAsyncExecution::ThreadPool, [this](const FDoneDelegate TestDone)
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

			IMPORT_CSV_ASYNC("", ",", "NONE", false, false, "Namespaces in CSV not found!");

			TestDone.Execute();
		});

		// ====================== 7. TESTS: VALID CSV WITH NAMESPACE AND EMPTY VALUES WITH FALLBACK ======================
		LatentIt(TEXT("VALID CSV WITH NAMESPACE AND EMPTY VALUES WITH FALLBACK"), EAsyncExecution::ThreadPool, [this](const FDoneDelegate TestDone)
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

				IMPORT_CSV_ASYNC("GlobalNamespace", ",", "FIRST_LANG", true, true, "");

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

				TestDone.Execute();
		});

		// ====================== 8. TESTS: VALID CSV WITH NAMESPACE AND EMPTY VALUES WITH FALLBACK ======================
		LatentIt(TEXT("VALID CSV WITH NAMESPACE AND EMPTY VALUES WITHOUT FALLBACK"), EAsyncExecution::ThreadPool, [this](const FDoneDelegate TestDone)
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

				IMPORT_CSV_ASYNC("GlobalNamespace", ",", "NONE", true, true, "");

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

				TestDone.Execute();
		});

		// ====================== 9. TESTS: VALID CSV WITH NAMESPACE AND EMPTY VALUES WITH KEY FALLBACK ======================
		LatentIt(TEXT("VALID CSV WITH NAMESPACE AND EMPTY VALUES WITH KEY FALLBACK"), EAsyncExecution::ThreadPool, [this](const FDoneDelegate TestDone)
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

				IMPORT_CSV_ASYNC("GlobalNamespace", ",", "KEY", true, true, "");

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

				TestDone.Execute();
		});

		// ====================== 10. TESTS: CSV CONVERT UNDERSCORES TO LINES ======================
		LatentIt(TEXT("CSV CONVERT UNDERSCORES TO LINES"), EAsyncExecution::ThreadPool, [this](const FDoneDelegate TestDone)
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

			IMPORT_CSV_ASYNC("GlobalNamespace", ",", "FIRST_LANG", true, true, "");

			TEST_LOCFILE_EXISTS("en");
			TEST_LOCFILE_EXISTS("pt-br");
			TEST_LOCFILE_EXISTS("zh-cn");

			TEST_TRANSLATION("en", "UI", "ButtonLabel", "Click Me");
			TEST_TRANSLATION("pt-br", "Audio", "MusicVolume", "M sica");
			TEST_TRANSLATION("zh-cn", "Settings", "Brightness", "Brightness");			

			TestDone.Execute();
		});

		// ====================== 11. TESTS: INVALID CSV DUPLICATED COLUMNS ======================
		LatentIt(TEXT("INVALID CSV DUPLICATED COLUMNS"), EAsyncExecution::ThreadPool, [this](const FDoneDelegate TestDone)
		{
			const FString CSV = TEXT("namespace,key,namespace,lang-en\n")
				TEXT("Menu,Start,Menu,Start Game\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(CSVName, CSV));

			IMPORT_CSV_ASYNC("", ",", "NONE", false, false, "Multiple 'namespace' columns found!");

			TestDone.Execute();
		});

		// ====================== 12. TESTS: INVALID CSV NO KEY COLUMNS ======================
		LatentIt(TEXT("INVALID CSV NO KEY COLUMNS"), EAsyncExecution::ThreadPool, [this](const FDoneDelegate TestDone)
		{
			const FString CSV = TEXT("namespace,value,lang-en,lang-fr\n")
				TEXT("MainMenu,Start Game,Commencer\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(CSVName, CSV));

			IMPORT_CSV_ASYNC("", ",", "NONE", false, false, "Key column not found!");

			TestDone.Execute();
		});

		// ====================== 13. TESTS: INVALID CSV NO LANG COLUMNS ======================
		LatentIt(TEXT("INVALID CSV NO LANG COLUMNS"), EAsyncExecution::ThreadPool, [this](const FDoneDelegate TestDone)
		{
			const FString CSV = TEXT("namespace,key\n")
				TEXT("MainMenu,Start\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(CSVName, CSV));

			IMPORT_CSV_ASYNC("", ",", "NONE", false, false, "No Lang column found!");

			TestDone.Execute();
		});

		// ====================== 14. TESTS: INVALID CSV INVALID NUM OF COLUMNS ======================
		LatentIt(TEXT("INVALID CSV INVALID NUM OF COLUMNS"), EAsyncExecution::ThreadPool, [this](const FDoneDelegate TestDone)
		{
			const FString CSV = TEXT("namespace,key,lang-en,lang-de\n")
				TEXT("Menu,Start,Start Game,Spiel starten\n")
				TEXT("Menu,Quit,Quit\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(CSVName, CSV));

			IMPORT_CSV_ASYNC("", ",", "NONE", false, false, "Every Column must have the same amount of values!");

			TestDone.Execute();
		});

		// ====================== 15. TESTS: INVALID CSV INVALID SEPARATOR ======================
		LatentIt(TEXT("INVALID CSV INVALID SEPARATOR"), EAsyncExecution::ThreadPool, [this](const FDoneDelegate TestDone)
		{
			FString CSV = TEXT("namespace,key,lang-en,lang-de\n")
				TEXT("Menu,Start,Start Game,Spiel starten\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(CSVName, CSV));

			IMPORT_CSV_ASYNC("", ";", "NONE", false, false, "Key column not found!");

			TestDone.Execute();
		});

		// ====================== 16. TESTS: CSV WITH SEMICOLON SEPARATOR ======================
		LatentIt(TEXT("CSV WITH SEMICOLON SEPARATOR"), EAsyncExecution::ThreadPool, [this](const FDoneDelegate TestDone)
		{
			const FString CSV = TEXT("namespace;key;lang-en;lang-sv;lang-no\n")
				TEXT("Navigation;North;North;North_SV;North_NO\n")
				TEXT("Navigation;South;South;South_SV;South_NO\n")
				TEXT("Navigation;East;East;East_SV;East_NO\n")
				TEXT("Navigation;West;West;West_SV;West_NO\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(CSVName, CSV));

			IMPORT_CSV_ASYNC("", ";", "NONE", false, true, "");

			TEST_LOCFILE_EXISTS("en");
			TEST_LOCFILE_EXISTS("sv");
			TEST_LOCFILE_EXISTS("no");

			TEST_TRANSLATION("en", "Navigation", "North", "North");
			TEST_TRANSLATION("sv", "Navigation", "North", "North_SV");
			TEST_TRANSLATION("no", "Navigation", "East", "East_NO");

			TestDone.Execute();
		});

		// ====================== 17. TESTS: CSV WITH TAB SEPARATOR ======================
		LatentIt(TEXT("CSV WITH TAB SEPARATOR"), EAsyncExecution::ThreadPool, [this](const FDoneDelegate TestDone)
		{
			const FString CSV = TEXT("namespace\tkey\tlang-en\tlang-da\tlang-fi\n")
				TEXT("Weather\tSunny\tSunny\tSunny_DA\tSunny_FI\n")
				TEXT("Weather\tRainy\tRainy\tRainy_DA\tRainy_FI\n")
				TEXT("Weather\tSnowy\tSnowy\tSnowy_DA\tSnowy_FI\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(CSVName, CSV));

			IMPORT_CSV_ASYNC("GlobalNamespace", "\t", "NONE", false, true, "");

			TEST_LOCFILE_EXISTS("en");
			TEST_LOCFILE_EXISTS("da");
			TEST_LOCFILE_EXISTS("fi");

			TEST_TRANSLATION("en", "Weather", "Sunny", "Sunny");
			TEST_TRANSLATION("da", "Weather", "Rainy", "Rainy_DA");
			TEST_TRANSLATION("fi", "Weather", "Snowy", "Snowy_FI");

			TestDone.Execute();
		});

		// ====================== 17. TESTS: CSV WITH MANY LANGUAGES ======================
		LatentIt(TEXT("CSV WITH MANY LANGUAGES"), EAsyncExecution::ThreadPool, [this](const FDoneDelegate TestDone)
		{
			const FString CSV = TEXT("namespace,key,lang-en,lang-es,lang-fr,lang-de,lang-it,lang-pt,lang-ja\n")
				TEXT("Status,Alive,Alive,Alive_ES,Alive_FR,Alive_DE,Alive_IT,Alive_PT,Alive_JA\n")
				TEXT("Status,Dead,Dead,Dead_ES,Dead_FR,Dead_DE,Dead_IT,Dead_PT,Dead_JA\n")
				TEXT("Status,Sleeping,Sleeping,Sleeping_ES,Sleeping_FR,Sleeping_DE,Sleeping_IT,Sleeping_PT,Sleeping_JA\n");

			TestTrue(TEXT("CSV file created"), FELTAutomationCommon::WriteTestCSV(CSVName, CSV));

			IMPORT_CSV_ASYNC("GlobalNamespace", ",", "NONE", false, true, "");

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

			TestDone.Execute();
		});

		// ====================== 18. TESTS: Util functions ======================
		LatentIt(TEXT("UTILS FUNCTIONS"), EAsyncExecution::ThreadPool, [this](const FDoneDelegate TestDone)
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

			IMPORT_CSV_ASYNC("GlobalNamespace", ",", "NONE", false, true, "");

			TEST_LOCFILE_EXISTS("en");
			TEST_LOCFILE_EXISTS("pl");
			TEST_LOCFILE_EXISTS("fr");
			TEST_LOCFILE_EXISTS("de");

			SWITCH_LANG("en");

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

			TestDone.Execute();
		});
	});
}

#endif // WITH_EDITOR

ELTTESTS_PRAGMA_ENABLE_OPTIMIZATION
