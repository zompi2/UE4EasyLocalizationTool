# Easy Localization Tool for Unreal Engine 4

This plugin introduces a way simplier method of localizing game made in Unreal Engine 4. 

It simply allow to import CSV file with localization.

## CSV format

The tool accepts a CSV file of a defined format:

![csvtesteg](https://user-images.githubusercontent.com/7863125/143495161-338a59b2-a41f-43a2-85cb-6a3b28078ab1.png)

``` csv
Namespace,Key,lang-en,lang-pl,lang-de,Comments
GAME,TEST_EXAMPLE,"Hello, world!",Witaj œwiecie!,Hallo Welt!,Anything you wish to type
```

* **Namespace** - a namespace in which current entry is located. This column is **optional**, but without it a **Global Namespace** must be defined.
* **Key** - a key of this entry, used later in text implementation.
* **lang-x** - a value in a **x** language. **x** is a language code, such as *en*, *pl*, *de*, etc.
* **Comments** - just a row for comments

> !!! IMPORTANT !!!  
> **Namespace** and **Key** must be first columns. The order of other columns doesn't matter.  
> Any column that isn't **Namespace**, **Key** or **lang-x** is ignored by the tool.

## Using the tool

To open the tool select `Window -> Easy Localization Tool` or use a shortcut `Alt + Shift + L`

![open](https://user-images.githubusercontent.com/7863125/143495187-8ceab883-f00f-463b-af32-0effd64f642b.png)

The following window should appear:

![elttoolwindow](https://user-images.githubusercontent.com/7863125/143495192-da02d1ac-cee3-4792-8a63-db6e4f748fda.png)

* **Localization Name** - Name of currently selected Localization. The game can have multiple localization directories.
* **Available Languages in Selected Localization** - list of language codes that are implemented in selected localization directory.
* **Available Languages** - list of language codes that are implemented by every localization directory.
* **Reimport on editor startup** - reimports the lastly selected localization with the last used CSV file when editor starts.
* **Localization Preview** - enabled the preview of the localization in the editor.
* **Override Language on Startup** - if enabled, when the game starts for the very first time the selected language will be used. Normally, the system language will be used or it will fallback to en.
* **CSV File** - CSV file to import.
* **Global Namespace** - this namespace will be assigned to every key in localization.


### Import

In order to import localization from CSV simply select the CSV file in the tool's window and click **Import**.  
If your CSV doesn't have a **Namespace** column, fill **Globl Namespace** property. That's it!

## Using localizations

In order to use a localized phrase type a key into the Text value:

[Image]

Then, set a Namespace and Key values for this Text:

[Image]

To use it in code use the following macro:

``` cpp
NSLOCTEXT("GAME", "TEST_EXAMPLE", "Hello, world!")
```


## Previewing localization

You can use **Localization Preview** option in the tool, or use the Preview option inside UMG Designer

[Image]

## Controlling localization

Easy Localization Tools comes with few handy functions to control localization in game:

### Get Available Languages

Returns a list of available language codes

![preview](https://user-images.githubusercontent.com/7863125/144485817-09998bdb-a27b-4d10-b139-4db271880996.png)
![open](https://user-images.githubusercontent.com/7863125/144485820-f855edc9-3def-4d5e-bf7c-42f4dcb458f4.png)
![elttoolwindow](https://user-images.githubusercontent.com/7863125/144485822-78bce0e4-094a-4189-a8df-50f9fad8aa9e.png)
![testex](https://user-images.githubusercontent.com/7863125/144485825-09cc1e5e-408d-48c4-adab-9fda9d7e51d7.png)
![testex2](https://user-images.githubusercontent.com/7863125/144485826-ede6dd9f-c7d8-42b5-a615-6f2d67f21816.png)
![elt1](https://user-images.githubusercontent.com/7863125/144485828-52dabe7a-ecc9-48db-a39b-a65a2f877987.png)
![elt2](https://user-images.githubusercontent.com/7863125/144485830-87f4cbe1-97f8-46cc-b740-40aad0e5c00e.png)
![elt3](https://user-images.githubusercontent.com/7863125/144485831-2e02ea40-96d5-4304-ba4d-ce1eeccd2825.png)
![elt4](https://user-images.githubusercontent.com/7863125/144485832-8c3c42e5-3e28-402c-a695-f2af39b9628e.png)
