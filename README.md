# Easy Localization Tool for Unreal Engine

This plugin introduces a way simplier method of localizing game made in Unreal Engine. 

It simply allow to import CSV file with localization.

The plugin has been tested on the Engine's versions: 4.27.2 and 5.2.0.

# Contact

If you have any question or suggestion regardles this plugin simply add an **Issue** to the github project or write an e-mail to me: **zompi2@gmail.com** I will try my best to answer it quickly :)

# Example Project

The example project wich uses this plugin can be found in **[this repository](https://github.com/zompi2/UE4EasyLocalizationToolExample)**.

# Table of content

- [CSV Format](#csv-format)
- [Using the Tool](#using-the-tool)
- [Import](#import)
- [Using Localizations](#using-localizations)
- [Cooking Localizations](#cooking-localizations)
- [LocText Struct](#loctext-struct)
- [Previewing Localizations](#previewing-localizations)
- [Controlling Localizations](#controlling-localizations)
- [Save File](#save-file)
- [Commandlet](#commandlet)
- [Utilities](#utilities)
- [Troubleshooting Errors](#troubleshooting-errors)
- [Special Thanks](#special-thanks)

## CSV Format

The tool accepts a CSV file of a defined format:

![csvtesteg](https://user-images.githubusercontent.com/7863125/143495161-338a59b2-a41f-43a2-85cb-6a3b28078ab1.png)

``` csv
Namespace,Key,lang-en,lang-pl,lang-de,Comments
GAME,TEST_EXAMPLE,"Hello, world!",Witaj świecie!,Hallo Welt!,Anything you wish to type
```

* **Namespace** - a namespace in which current entry is located. This column is **optional**, but without it a **Global Namespace** must be defined.
* **Key** - a key of this entry, used later in text implementation.
* **lang-x** - a value in a **x** language. **x** is a language code, such as *en*, *pl*, *de*, etc.
> Unreal Engine uses ICU codes. The full list of them can be found here: https://www.localeplanet.com/icu/
* **Comments** - just a row for comments

> **!!! VERY IMPORTANT !!!**  
> 
> The newline character for every entry **MUST** be a **CRLF**, otherwise UE4's Slate will constantly try to replace the given text Source, leading to errors!   
> **Namespace** and **Key** must be first columns. The order of other columns doesn't matter.  
> Any column that isn't **Namespace**, **Key** or **lang-x** is ignored by the tool.

[Back to top](#table-of-content)

## Using the Tool

To open the tool select `Window -> Easy Localization Tool` or use a shortcut `Alt + Shift + L`

![open](https://user-images.githubusercontent.com/7863125/143495187-8ceab883-f00f-463b-af32-0effd64f642b.png)

The following window should appear:

![eltimg](https://github.com/zompi2/UE4EasyLocalizationTool/assets/7863125/dd0e7a61-34ca-42f8-811b-5b30bf4d7a15)

* **Localization Name** - Name of currently selected Localization. The game can have multiple localization directories.
* **Available Languages in Selected Localization** - list of language codes that are implemented in selected localization directory.
* **Available Languages** - list of language codes that are implemented by every localization directory.
* **Reimport on editor startup** - reimports the lastly selected localization with the last used CSV file when editor starts.
* **Localization Preview** - enabled the preview of the localization in the editor.
* **Manually Set Last Language** - if enabled it won't save and load lastly set language automatically. 
* **Override Language on Startup** - if enabled, when the game starts for the very first time the selected language will be used. Normally, the system language will be used or it will fallback to `en`.
* **CSV File** - CSV file to import.
* **Global Namespace** - this namespace will be assigned to every key in localization.

[Back to top](#table-of-content)

## Import

In order to import localization from CSV simply select the CSV file in the tool's window and click **Import**.  
If your CSV doesn't have a **Namespace** column, fill **Globl Namespace** property. That's it!

[Back to top](#table-of-content)

## Using Localizations

In order to use a localized phrase type a **KEY** into the Text form and then set a Namespace and Key values for this Text:

![testexampleloc](https://github.com/zompi2/UE4EasyLocalizationTool/assets/7863125/e28d1a4f-cd9f-4ce8-a966-e4dd388e1c67)

To use it in a c++ code use the following macro:

``` cpp
NSLOCTEXT("GAME", "TEST_EXAMPLE", "TEST_EXAMPLE")
```

[Back to top](#table-of-content)

## Cooking localizations
In order to make localization work on standnalone build there are two options in `Project Settings` in  the `Packaging` section (under `Advanced options`!) that need to be set:
* `Internationalization Support` set to `All`:  
![int support](https://github.com/zompi2/UE4EasyLocalizationTool/assets/7863125/fbac259c-fec0-4656-b0f1-d830298385b9)
* Localization directory added to the `Addidiontal Non-Asset Directories to Copy` list:  
![loccopy](https://github.com/zompi2/UE4EasyLocalizationTool/assets/7863125/0f244d7c-2744-411f-ac77-b85dedc46e0e)

[Back to top](#table-of-content)

## LocText Struct

There is an unwanted behaviour of `FText` - it keeps creating a new Key for every child Blueprint it's in.  
It means that if there is a `FText` in a Blueprint, this `FText` will not be valid in this Blueprints's children.  

In order to workaround this issue the `FLocText` has been introduce. It is a structure that holds Namespace and Key and it can give a corresponding to it `FText`. The structrue is immune to the inheritence problems.  

``` cpp
FLocText LocalizedStruct("GAME", "TEST_EXAMPLE");
FText LocalizedText = LocalizedStruct.GetText();
FString LocalizedString = LocalizedStruct.ToString();
```
Struct is supported in Blueprints too, of course.

![locstructimg](https://user-images.githubusercontent.com/7863125/147063762-8501121a-50f9-4dca-bfd4-fad096b63437.png)
![locstructbpcast](https://user-images.githubusercontent.com/7863125/147063770-e669c169-273b-4b09-97df-14ba834cf8c8.png)

[Back to top](#table-of-content)

## Previewing Localizations

You can use **Localization Preview** option in the tool, or use the Preview option inside UMG Designer

![preview](https://user-images.githubusercontent.com/7863125/144485817-09998bdb-a27b-4d10-b139-4db271880996.png)

[Back to top](#table-of-content)

## Controlling Localizations

Easy Localization Tool comes with few handy functions to control localization in game:

#### Get Available Languages

Returns a list of available language codes.

![elt1](https://user-images.githubusercontent.com/7863125/144485828-52dabe7a-ecc9-48db-a39b-a65a2f877987.png)

``` cpp
GetELT()->GetAvailableLanguages();
```

#### Get Current Language

Returns a code of the language which is currently in use.

![elt2](https://user-images.githubusercontent.com/7863125/144485830-87f4cbe1-97f8-46cc-b740-40aad0e5c00e.png)

``` cpp
GetELT()->GetCurrentLanguage();
```

#### Can Set Language

Checks if a language with the given language code can be set.

![elt3](https://user-images.githubusercontent.com/7863125/144485831-2e02ea40-96d5-4304-ba4d-ce1eeccd2825.png)

``` cpp
GetELT()->CanSetLanguage("en");
```

#### Set Language

Sets a language with the given language code. Returns false if the language couldn't be set.

![elt4](https://user-images.githubusercontent.com/7863125/144485832-8c3c42e5-3e28-402c-a695-f2af39b9628e.png)

``` cpp
GetELT()->SetLanguage("en");
```

#### On Text Localization Changed Event

Allow to bind delegate which will run every time a Text Localization has been changed.

![locchange](https://user-images.githubusercontent.com/7863125/152312310-7beafb2b-616b-4cfe-b757-305519985721.png)

``` cpp
GetELT()->OnTextLocalizationChanged.AddDynamic(this, &UMyObject::OnTextLocalizationChangedUFunc);
```
or
``` cpp
GetELT()->OnTextLocalizationChangedStatic.AddLambda([this]()
{
    // Stuff to do when localization has changed.
});
```

#### Refresh Language Resources
There might be a rare situation when texts won't be displayed in a localized form.  
There is a high chance running this function will fix this issue.

![ref](https://github.com/zompi2/UE4EasyLocalizationTool/assets/7863125/e06b435e-a4bd-4839-8b56-cc5cdc8f4c9a)

``` cpp
GetELT()->RefreshLanguageResources();
```

[Back to top](#table-of-content)

## Save File

Easy Localization Tool saves lastly used language and sets it when starting a game.  
The save file is located in `MyGame\Saved\SaveGames\ELTSave.sav`  

> The lastly used language will not be saved and loaded when `Manually Set Last Language` is enabled! This option might be needed if the game is not allowed to read save files on startup (i.e.: in console builds). In such situations use `SetLanguage` to setup desired lanugage when you can.

[Back to top](#table-of-content)

## Commandlet

You can generate localization without starting the editor, but via command line.  
It might be useful when building a game via CI platform like Jenkins.

You can use the following script (win64) to generate localization files:

```
set UE4_PATH=C:\E4
set PROJECT_PATH=C:\MyGame

call %UE4_PATH%\Engine\Binaries\Win64\UE4Editor-Cmd.exe %PROJECT_PATH%\MyGame.uproject -run=ELTCommandlet -CSVPath=%PROJECT_PATH%\Lockit.csv -LocPath=%PROJECT_PATH%\Content\Localization\Game -Namespace=GAME
```

Where:

* **-CSVPath** - is a path to the csv file to import.
* **-LocPath** - is a directory where localization files should be stored.
* **-Namespace** - optional parameter which sets a **Global Namespace** value.

[Back to top](#table-of-content)

## Utilities

#### Get Text Data

Gets a Package, Namespace, Key and Source info from FText.

![getinfo](https://user-images.githubusercontent.com/7863125/168045473-d05f9138-736d-4c3c-96d8-1b5b8ade2556.png)

#### Are Text Keys Equal

Checks if two FTexts' keys are the same. If at least one of them has invalid key it will return false.

![arete](https://user-images.githubusercontent.com/7863125/200557176-c3acdf6b-e1c4-433d-a28d-aafefd336527.png)

#### Validate Text

*This is Editor Scripting only utility.*  
It checks if the given FText is properly localized - it means it checks if Source and Key are the same.  
It will return false if given FText is empty or if it is Culture Invariant.

![validtxt2](https://user-images.githubusercontent.com/7863125/168045759-8fd19a3e-9c39-4363-9664-a76945afa768.png)

#### Replace Text

*This is Editor Scripting only utility.*  
It replaces the OriginText with ReplaceWithText while keeping the OriginText's package id.  
Use this instead of normal FText copy operator when using editor scripts, because it copies the package id which leads to broken FText localizations.  
OriginTextOwner might be required when the OriginText is empty and has not package id assigned yet.

![image](https://github.com/zompi2/UE4EasyLocalizationTool/assets/7863125/492dc4b7-3e2a-4202-a461-182f08ea226f)

[Back to top](#table-of-content)

# Troubleshooting Errors
CSV must have the same amount of entries in every row (entries can be empty, but they must exist). There also should be a Namespace column if global namespace is not set. When importing CSV you might encounter such errors:
* `ERROR: Failed loading CSV! Trying to add a word: 'Dies, ist ein Beispiel, mit Interpunktionen!' to a row 3, column 5 (counting from 1) while there are 4 columns.` - this error might encounter when a row has more entries than the first "header" row. Check if the first row has every entry required and if any other row has no more entries than the "header" row.
*  `ERROR: Invalid CSV! Column 5 (counting from 1) has 6 values while Column 1 has 7 values. Every Column must have the same amount of values!` - this error might encounter when there are less entries in a row than in a "header" row.
* `ERROR: CSV file not found!` - this error will encounter when the given CSV file does not exist.
* `ERROR: Namespaces in CSV not found!` - this error will encounter when CSV has no `Namespace` Column and the `Global Namespace` is not set.
* `ERROR: Namespace in row 2 (counting from 1) is empty!` - this error will encounter when CSV has `Namespace` Column, but there is an empty entry under it and the `Global Namespace` is not set.
* `ERROR: CSV has not enough Columns!` - this error will enncounter when CSV is empty or if it has only one Column. This tool requires at least two Columns in a CSV file to work.

# Extra Links
* [Blog Entry](https://zompidev.blogspot.com/2022/02/easy-localization-tool-for-ue4.html)  
* [Reddit Post](https://www.reddit.com/r/unrealengine/comments/ymg86r/easy_localization_tool)

[Back to top](#table-of-content)

# Special Thanks

I want to send special thanks to Monika, because she always supports me and believes in me, to Pawel, for allowing me to test this plugin on his project, to [cziter15](https://github.com/cziter15) for the idea of how it should work and to everyone that contributed to this project.  
Also, I want to thank You for using this plugin! It is very important for me that my work is useful for someone!  
Happy coding!

[Back to top](#table-of-content)

