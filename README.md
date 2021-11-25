# Easy Localization Tool for Unreal Engine 4

This plugin introduces a way simplier method of localizing game made in Unreal Engine 4. 

It simply allow to import CSV file with localization.

## CSV format

The tool accepts a CSV file of a defined format:

**[IMG]**

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
> Any column that isn't **Namespace**, **Key** and **lang-x** is ignored by a tool.

## Using the tool

To open the tool select `Windows -> Easy Localization Tool` or use a shortcut `Alt + Shift + L`

**[IMG]**

The following window should appear:

**[IMG]**

* **Localization Name** - Name of currently selected Localization. The game can have multiple localization directories.
* **Available Languages in Selected Localization** - list of language codes that are implemented in selected localization directory.
* **Available Languages** - list of language codes that are implemented by every localization directory.
* **Reimport on editor startup** - reimports the lastly selected localization with the last used CSV file when editor starts.
* **Localization Preview** - enabled the preview of the localization in the editor.
* **Override Language on Startup** - if enabled, when the game starts for the very first time the selected language will be used. Normally, the system language will be used or it will fallback to en.
* **CSV File** - CSV file to import.
* **Global Namespace** - this namespace will be assigned to every key in localization.


**TODO** this documentation is not finished.