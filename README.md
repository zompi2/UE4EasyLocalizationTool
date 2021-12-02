# Easy Localization Tool for Unreal Engine 4

This plugin introduces a way simplier method of localizing game made in Unreal Engine 4. 

It simply allow to import CSV file with localization.

# Contact

If you have any question or suggestion regardles this plugin simply add an **Issue** to the github project or write an e-mail to me: **zompi2@gmail.com** I will try my best to answer it quickly :)

# Table of content

- [CSV Format](#csv-format)
- [Using the Tool](#using-the-tool)
- [Import](#import)
- [Using Localizations](#using-localizations)
- [Previewing Localizations](#previewing-localizations)
- [Controlling Localizations](#controlling-localizations)
- [Save File](#save-file)
- [Commandlet](#commandlet)
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
* **Comments** - just a row for comments

> !!! IMPORTANT !!!  
> **Namespace** and **Key** must be first columns. The order of other columns doesn't matter.  
> Any column that isn't **Namespace**, **Key** or **lang-x** is ignored by the tool.

[Back to top](#table-of-content)

## Using the Tool

To open the tool select `Window -> Easy Localization Tool` or use a shortcut `Alt + Shift + L`

![open](https://user-images.githubusercontent.com/7863125/143495187-8ceab883-f00f-463b-af32-0effd64f642b.png)

The following window should appear:

![elttoolwindow](https://user-images.githubusercontent.com/7863125/143495192-da02d1ac-cee3-4792-8a63-db6e4f748fda.png)

* **Localization Name** - Name of currently selected Localization. The game can have multiple localization directories.
* **Available Languages in Selected Localization** - list of language codes that are implemented in selected localization directory.
* **Available Languages** - list of language codes that are implemented by every localization directory.
* **Reimport on editor startup** - reimports the lastly selected localization with the last used CSV file when editor starts.
* **Localization Preview** - enabled the preview of the localization in the editor.
* **Override Language on Startup** - if enabled, when the game starts for the very first time the selected language will be used. Normally, the system language will be used or it will fallback to `en`.
* **CSV File** - CSV file to import.
* **Global Namespace** - this namespace will be assigned to every key in localization.

[Back to top](#table-of-content)

## Import

In order to import localization from CSV simply select the CSV file in the tool's window and click **Import**.  
If your CSV doesn't have a **Namespace** column, fill **Globl Namespace** property. That's it!

[Back to top](#table-of-content)

## Using Localizations

In order to use a localized phrase type a key into the Text value:

![testex](https://user-images.githubusercontent.com/7863125/144485825-09cc1e5e-408d-48c4-adab-9fda9d7e51d7.png)

Then, set a Namespace and Key values for this Text:

![testex2](https://user-images.githubusercontent.com/7863125/144485826-ede6dd9f-c7d8-42b5-a615-6f2d67f21816.png)

To use it in a c++ code use the following macro:

``` cpp
NSLOCTEXT("GAME", "TEST_EXAMPLE", "Hello, world!")
```

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
TArray<FString> UELT::GetAvailableLanguages();
```

#### Get Current Language

Returns a code of the language which is currently in use.

![elt2](https://user-images.githubusercontent.com/7863125/144485830-87f4cbe1-97f8-46cc-b740-40aad0e5c00e.png)

``` cpp
FString UELT::GetCurrentLanguage();
```

#### Can Set Language

Checks if a language with the given language code can be set.

![elt3](https://user-images.githubusercontent.com/7863125/144485831-2e02ea40-96d5-4304-ba4d-ce1eeccd2825.png)

``` cpp
bool UELT::CanSetLanguage(const FString& Language);
```

#### Set Language

Sets a language with the given language code.

![elt4](https://user-images.githubusercontent.com/7863125/144485832-8c3c42e5-3e28-402c-a695-f2af39b9628e.png)

``` cpp
void UELT::SetLanguage(const FString& Language);
```

[Back to top](#table-of-content)

## Save File

Easy Localization Tool saves lastly used language and sets it when starting a game.  
The save file is located in `MyGame\Saved\SaveGames\ELTSave.sav`

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

# Special Thanks

I want to send special thanks to Monika, because she always supports me and believes in me, to Pawel, for allowing me to test this plugin on his project, to [cziter15](@cziter15) for the idea of how it should work and to everyone that contributed to this project.  
Also, I want to thank You for using this plugin! It is very important for me that my work is useful for someone!  
Happy coding!

[Back to top](#table-of-content)

