// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#include "SELTEditorWidget.h"
#include "ELTEditorWidget.h"
#include "Widgets/SCanvas.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Layout/SConstraintCanvas.h"

ELTEDITOR_PRAGMA_DISABLE_OPTIMIZATION

void SELTEditorWidget::Construct(const FArguments& InArgs)
{
	FallbackWhenEmptyAvailable.Empty();
	FallbackWhenEmptyAvailable.Add(MakeShareable(new FString(TEXT("NONE"))));
	FallbackWhenEmptyAvailable.Add(MakeShareable(new FString(TEXT("FIRST_LANG"))));
	FallbackWhenEmptyAvailable.Add(MakeShareable(new FString(TEXT("KEY"))));
	SelectedFallbackWhenEmpty = FallbackWhenEmptyAvailable[0];

	SpacerBrush.SetImageSize(FVector2D(350.f, 1.f));
	SpacerBrush.TintColor = FSlateColor(FLinearColor(.62f,.62f,.62f,1.f));

	SUserWidget::Construct(SUserWidget::FArguments()
	[
		SNew(SConstraintCanvas)
			+SConstraintCanvas::Slot()
			.Anchors(FAnchors(0.f, 0.f, 1.f, 1.f))
			.Offset(FMargin(20.f))
			[
				SNew(SVerticalBox)
					// > Title Text ===========
					+SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(STextBlock)
							.Font(FCoreStyle::GetDefaultFontStyle("Bold", 20))
							.Text(INVTEXT("Easy Localization Tool"))
					]
					// > Version ==============
					+SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(STextBlock)
							.Font(FCoreStyle::GetDefaultFontStyle("Light", 8))
							.Text_Lambda([this]() -> FText
							{
								if (WidgetController.IsValid())
								{
									return FText::FromString(TEXT("v.") + WidgetController->GetPluginVersion());
								}
								return FText::GetEmpty();
							})
					]
					// > Spacer ================
					+SVerticalBox::Slot()
					.AutoHeight()
					.Padding(FMargin(0.f, 4.f, 0.f, 4.f))
					.HAlign(EHorizontalAlignment::HAlign_Left)
					.VAlign(EVerticalAlignment::VAlign_Center)
					[
						SNew(SImage).Image(&SpacerBrush)
					]
					// > Localiation Paths Box
					+SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SVerticalBox)
					// >>>> Localization Path selection list
							+SVerticalBox::Slot()
							.AutoHeight()
							.HAlign(EHorizontalAlignment::HAlign_Left)
							[
								SNew(SComboBox<TSharedPtr<FString>>)
									.OptionsSource(&LocPathsList)
									.OnGenerateWidget_Lambda([this](TSharedPtr<FString> InItem) -> TSharedRef<SWidget>
									{
										return SNew(STextBlock).Text(FText::FromString(*InItem));
									})
									.OnSelectionChanged_Lambda([this](TSharedPtr<FString> Item, ESelectInfo::Type SelectInfo) -> void
									{
										SelectedLocPath = Item;
										if (WidgetController.IsValid())
										{
											WidgetController->OnLocalizationPathSelected(*Item);
										}
									})
								[
									SNew(STextBlock)
										.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
										.Text_Lambda([this]() -> FText
										{
											if (SelectedLocPath.IsValid())
											{
												return FText::FromString(*SelectedLocPath);
											}
											return FText::GetEmpty();
										})
								]
							]
					// >>>> Localization Name box
							+SVerticalBox::Slot()
							.AutoHeight()
							[
								SNew(SHorizontalBox)
					// >>>>>>>> Localization Name label
								+SHorizontalBox::Slot()
								.AutoWidth()
								.Padding(FMargin(0.f, 0.f, 20.f, 0.f))
								[
									SNew(STextBlock)
										.Font(FCoreStyle::GetDefaultFontStyle("Light", 12))
										.Text(INVTEXT("Localization name:"))
								]
					// >>>>>>>> Localization Name value
								+SHorizontalBox::Slot()
								.AutoWidth()
								.Padding(FMargin(0.f, 0.f, 20.f, 0.f))
								[
									SNew(STextBlock)
										.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
										.Text_Lambda([this]() -> FText
										{
											return FText::FromString(CurrentLocName);
										})
								]
							]
					]
					// > Spacer ================
					+SVerticalBox::Slot()
					.AutoHeight()
					.Padding(FMargin(0.f, 4.f, 0.f, 4.f))
					.HAlign(EHorizontalAlignment::HAlign_Left)
					.VAlign(EVerticalAlignment::VAlign_Center)
					[
						SNew(SImage).Image(&SpacerBrush)
					]
					// > Available Langs In Selected Localization Box ================
					+SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SVerticalBox)
					// >>>> Available Langs In Selected Localization Label
						+SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(STextBlock)
								.Font(FCoreStyle::GetDefaultFontStyle("Light", 12))
								.Text(INVTEXT("Available Languages In Selected Localization:"))
						]
					// >>>> Available Langs In Selected Localization Value
						+SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(STextBlock)
								.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
								.Text_Lambda([this]() -> FText
								{
									return FText::FromString(AvailableLangsInLocFile);
								})
						]
					]
					// > Available Langs Box ================
					+SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SVerticalBox)
					// >>>> Available Langs Label
						+SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(STextBlock)
								.Font(FCoreStyle::GetDefaultFontStyle("Light", 12))
								.Text(INVTEXT("Available Languages:"))
						]
					// >>>> Localization Names value
						+SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(STextBlock)
								.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
								.Text_Lambda([this]() -> FText
								{
									return FText::FromString(AvailableLangs);
								})
						]
					]
					// > Spacer ================
					+SVerticalBox::Slot()
					.AutoHeight()
					.Padding(FMargin(0.f, 4.f, 0.f, 4.f))
					.HAlign(EHorizontalAlignment::HAlign_Left)
					.VAlign(EVerticalAlignment::VAlign_Center)
					[
						SNew(SImage).Image(&SpacerBrush)
					]
					// > Reimport on editor startup Box ================
					+SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SHorizontalBox)
					// >>>> Reimport on editor startup Label
						+SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(STextBlock)
								.Font(FCoreStyle::GetDefaultFontStyle("Light", 12))
								.Text(INVTEXT("Reimport on editor startup:"))
						]
					// >>>> Reimport on editor startup checkbox
						+SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SCheckBox)
								.IsChecked_Lambda([this]() -> ECheckBoxState
								{
									return bReimportAtEditorStartup_Chkbox ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
								})
								.OnCheckStateChanged_Lambda([this](ECheckBoxState InCheckBoxState) -> void
								{
									bReimportAtEditorStartup_Chkbox = (InCheckBoxState == ECheckBoxState::Checked);
									if (WidgetController.IsValid())
									{
										WidgetController->OnReimportAtEditorStartupChanged(bReimportAtEditorStartup_Chkbox);
									}
								})
						]
					]
					// > Localization Preview Box ================
					+SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SHorizontalBox)
					// >>>> Localization Preview Label
						+SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(STextBlock)
								.Font(FCoreStyle::GetDefaultFontStyle("Light", 12))
								.Text(INVTEXT("Localization Preview:"))
						]
					// >>>> Localization Preview Checkbox
						+SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SCheckBox)
								.IsChecked_Lambda([this]() -> ECheckBoxState
								{
									return bIsLocalisationPreviewEnabled_Chkbox ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
								})
								.OnCheckStateChanged_Lambda([this](ECheckBoxState InCheckBoxState) -> void
								{
									bIsLocalisationPreviewEnabled_Chkbox = (InCheckBoxState == ECheckBoxState::Checked);
									if (WidgetController.IsValid())
									{
										WidgetController->OnLocalizationPreviewChanged(bIsLocalisationPreviewEnabled_Chkbox);
									}
								})
						]
					// >>>> Localization Preview List
						+SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SComboBox<TSharedPtr<FString>>)
								.OptionsSource(&PreviewsAvailables)
								.OnGenerateWidget_Lambda([this](TSharedPtr<FString> InItem) -> TSharedRef<SWidget>
								{
									return SNew(STextBlock).Text(FText::FromString(*InItem));
								})
								.OnSelectionChanged_Lambda([this](TSharedPtr<FString> Item, ESelectInfo::Type SelectInfo) -> void
								{
									SelectedPreviewLang = Item;
									if (WidgetController.IsValid())
									{
										WidgetController->OnLocalizationPreviewLangChanged(*Item);
									}
								})
							[
								SNew(STextBlock)
									.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
									.Text_Lambda([this]() -> FText
										{
											if (SelectedPreviewLang.IsValid())
											{
												return FText::FromString(*SelectedPreviewLang);
											}
											return FText::GetEmpty();
										})
							]
						]
					]
					// > Manually Set Last Language Box ================
					+SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SHorizontalBox)
					// >>>> Manually Set Last Language Label
						+SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(STextBlock)
								.Font(FCoreStyle::GetDefaultFontStyle("Light", 12))
								.Text(INVTEXT("Manually Set Last Language:"))
						]
					// >>>> Manually Set Last Language checkbox
						+SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SCheckBox)
								.IsChecked_Lambda([this]() -> ECheckBoxState
								{
									return bManuallySetLastLanguage_Chkbox ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
								})
								.OnCheckStateChanged_Lambda([this](ECheckBoxState InCheckBoxState) -> void
								{
									bManuallySetLastLanguage_Chkbox = (InCheckBoxState == ECheckBoxState::Checked);
									if (WidgetController.IsValid())
									{
										WidgetController->OnManuallySetLastUsedLanguageChanged(bManuallySetLastLanguage_Chkbox);
									}
								})
						]
					]
					// > Override Language on Startup Box ================
					+SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SHorizontalBox)
					// >>>> Override Language on Startup Label
						+SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(STextBlock)
								.Font(FCoreStyle::GetDefaultFontStyle("Light", 12))
								.Text(INVTEXT("Override Language on Startup:"))
						]
					// >>>> Override Language on Startup Checkbox
						+SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SCheckBox)
								.IsChecked_Lambda([this]() -> ECheckBoxState
								{
									return bOverrideLanguageOnStartup_Chkbox ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
								})
								.OnCheckStateChanged_Lambda([this](ECheckBoxState InCheckBoxState) -> void
								{
									bOverrideLanguageOnStartup_Chkbox = (InCheckBoxState == ECheckBoxState::Checked);
									if (WidgetController.IsValid())
									{
										WidgetController->OnLocalizationOnFirstRun(bOverrideLanguageOnStartup_Chkbox);
									}
								})
						]
					// >>>> Override Language on Startup List
						+SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SComboBox<TSharedPtr<FString>>)
								.OptionsSource(&LanguageOverridesAvailable)
								.OnGenerateWidget_Lambda([this](TSharedPtr<FString> InItem) -> TSharedRef<SWidget>
								{
									return SNew(STextBlock).Text(FText::FromString(*InItem));
								})
								.OnSelectionChanged_Lambda([this](TSharedPtr<FString> Item, ESelectInfo::Type SelectInfo) -> void
								{
									SelectedLanguageOverride = Item;
									if (WidgetController.IsValid())
									{
										WidgetController->OnLocalizationOnFirstRunLangChanged(*Item);
									}
								})
							[
								SNew(STextBlock)
									.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
									.Text_Lambda([this]() -> FText
										{
											if (SelectedLanguageOverride.IsValid())
											{
												return FText::FromString(*SelectedLanguageOverride);
											}
											return FText::GetEmpty();
										})
							]
						]
					]
					// > Separator Box ================
					+SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SVerticalBox)
					// >>>> Separator Label
						+SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(STextBlock)
								.Font(FCoreStyle::GetDefaultFontStyle("Light", 12))
								.Text(INVTEXT("Separator:"))
						]
					// >>>> Separator Value
						+SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(EHorizontalAlignment::HAlign_Left)
						[
							SNew(SEditableTextBox)
								.Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
								.MinDesiredWidth(256.f)
								.Text_Lambda([this]() -> FText
								{
									return FText::FromString(SeparatorValue);
								})
								.OnTextCommitted_Lambda([this](const FText& NewText, ETextCommit::Type CommitType) -> void
								{
									SeparatorValue = NewText.ToString();
									if (WidgetController.IsValid())
									{
										WidgetController->OnSeparatorChanged(SeparatorValue);
									}
								})
						]
					]
					// > Fallback when empty Box ================
					+SVerticalBox::Slot()
					.AutoHeight()
					.Padding(FMargin(0.f, 4.f, 0.f, 0.f))
					[
						SNew(SHorizontalBox)
						.ToolTipText(INVTEXT("\
When the entry is empty should it fill it with a fallback value?\n\
NONE - no fallback\n\
FIRST_LANG - use value of the first language.If that value is empty use Key\n\
KEY - use the key of this entry"))
					// >>>> Fallback when empty Label
						+SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(STextBlock)
								.Font(FCoreStyle::GetDefaultFontStyle("Light", 12))
								.Text(INVTEXT("Fallback when empty:"))
						]
					// >>>> Fallback when empty List
						+SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SComboBox<TSharedPtr<FString>>)
								.OptionsSource(&FallbackWhenEmptyAvailable)
								.OnGenerateWidget_Lambda([this](TSharedPtr<FString> InItem) -> TSharedRef<SWidget>
								{
									return SNew(STextBlock).Text(FText::FromString(*InItem));
								})
								.OnSelectionChanged_Lambda([this](TSharedPtr<FString> Item, ESelectInfo::Type SelectInfo) -> void
								{
									SelectedFallbackWhenEmpty = Item;
									if (SelectedFallbackWhenEmpty.IsValid())
									{
										// TODO: Uncomment when merged with this feature.
										// WidgetController->OnFallbackWhenEmptyChanged(*Item);
									}
								})
							[
								SNew(STextBlock)
									.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
									.Text_Lambda([this]() -> FText
										{
											if (SelectedFallbackWhenEmpty.IsValid())
											{
												return FText::FromString(*SelectedFallbackWhenEmpty);
											}
											return FText::GetEmpty();
										})
							]
						]
					]
					// > Spacer ================
					+SVerticalBox::Slot()
					.AutoHeight()
					.Padding(FMargin(0.f, 4.f, 0.f, 4.f))
					.HAlign(EHorizontalAlignment::HAlign_Left)
					.VAlign(EVerticalAlignment::VAlign_Center)
					[
						SNew(SImage).Image(&SpacerBrush)
					]
					// > CSV files Box ================
					+SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SVerticalBox)
					// >>>> CSV files list box
						+SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(SVerticalBox)
					// >>>>>>> CSV files list label
							+SVerticalBox::Slot()
							.AutoHeight()
							[
								SNew(STextBlock)
									.Font(FCoreStyle::GetDefaultFontStyle("Light", 12))
									.Text(INVTEXT("CSV file:"))
							]
					// >>>>>>> CSV files list value
							+SVerticalBox::Slot()
							.AutoHeight()
							[
								SNew(STextBlock)
									.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
									.Text_Lambda([this]() -> FText
									{
										return FText::FromString(CSVFiles);
									})
							]
						]
						+SVerticalBox::Slot()
					// >>>> CSV select files box
						.AutoHeight()
						[
							SNew(SHorizontalBox)
					// >>>>>>> CSV select files button
							+ SHorizontalBox::Slot()
							.AutoWidth()
							[
								SNew(SButton)
									.Text(INVTEXT("Select"))
									.OnClicked_Lambda([this]() -> FReply
									{
										if (WidgetController.IsValid())
										{
											WidgetController->SelectNewCSVPath();
										}
										return FReply::Handled();
									})
							]
					// >>>>>>> CSV import files button
							+SHorizontalBox::Slot()
							.AutoWidth()
							[
								SNew(SButton)
									.Text(INVTEXT("Import"))
									.OnClicked_Lambda([this]() -> FReply
									{
										if (WidgetController.IsValid())
										{
											WidgetController->GenerateLocFiles();
										}
										return FReply::Handled();
									})
							]
						]
					]
					// > Global namespace box ================
					+SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SVerticalBox)
					// >>>> Global namespace label ================
						+SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(STextBlock)
								.Font(FCoreStyle::GetDefaultFontStyle("Light", 12))
								.Text(INVTEXT("Global namespace:"))
						]
					// >>>> Global namespace value ================
						+SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(EHorizontalAlignment::HAlign_Left)
						[
							SNew(SEditableTextBox)
								.Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
								.MinDesiredWidth(256.f)
								.Text_Lambda([this]() -> FText
								{
									return FText::FromString(GlobalNamespaceValue);
								})
								.OnTextCommitted_Lambda([this](const FText& NewText, ETextCommit::Type CommitType) -> void
								{
									GlobalNamespaceValue = NewText.ToString();
									if (WidgetController.IsValid())
									{
										WidgetController->OnGlobalNamespaceChanged(GlobalNamespaceValue);
									}
								})
						]
					]
					// > Spacer ================
					+SVerticalBox::Slot()
					.AutoHeight()
					.Padding(FMargin(0.f, 4.f, 0.f, 4.f))
					.HAlign(EHorizontalAlignment::HAlign_Left)
					.VAlign(EVerticalAlignment::VAlign_Center)
					[
						SNew(SImage).Image(&SpacerBrush)
					]
					// > Log Debug Box ================
					+SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SHorizontalBox)
					// >>>> Log Debug Label
						+SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(STextBlock)
								.Font(FCoreStyle::GetDefaultFontStyle("Light", 12))
								.Text(INVTEXT("Log Debug:"))
						]
					// >>>> Log Debug checkbox
						+SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SCheckBox)
								.IsChecked_Lambda([this]() -> ECheckBoxState
								{
									return bLogDebug_Chkbox ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
								})
								.OnCheckStateChanged_Lambda([this](ECheckBoxState InCheckBoxState) -> void
								{
									bLogDebug_Chkbox = (InCheckBoxState == ECheckBoxState::Checked);
									if (WidgetController.IsValid())
									{
										WidgetController->OnLogDebugChanged(bLogDebug_Chkbox);
									}
								})
						]
					]
					// > Preview In UI Box ================
					+SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SHorizontalBox)
							.Visibility_Lambda([this]() -> EVisibility
							{
								if (WidgetController.IsValid())
								{
									if (WidgetController->IsPreviewInUISupported())
									{
										return EVisibility::Visible;
									}
								}
								return EVisibility::Collapsed;
							})
					// >>>> Preview In UI Label
						+SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(STextBlock)
								.Font(FCoreStyle::GetDefaultFontStyle("Light", 12))
								.Text(INVTEXT("Show preview in UI:"))
						]
					// >>>> Preview In UI checkbox
						+SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SCheckBox)
								.IsChecked_Lambda([this]() -> ECheckBoxState
								{
									return bPreviewInUI_Chkbox ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
								})
								.OnCheckStateChanged_Lambda([this](ECheckBoxState InCheckBoxState) -> void
								{
									bPreviewInUI_Chkbox = (InCheckBoxState == ECheckBoxState::Checked);
									if (WidgetController.IsValid())
									{
										WidgetController->OnPreviewInUIChanged(bPreviewInUI_Chkbox);
									}
								})
						]
					]
			]
	]);
}

TSharedRef<SELTEditorWidget> SELTEditorWidget::New()
{
	return MakeShareable(new SELTEditorWidget());
}


void SELTEditorWidget::FillLocalizationPaths(const TArray<FString>& Paths)
{
	LocPathsList.Empty();
	for (const FString& Path : Paths)
	{
		LocPathsList.Add(MakeShared<FString>(Path));
	}
	SelectedLocPath = LocPathsList.Num() > 0 ? LocPathsList[0] : nullptr;
	if (WidgetController.IsValid())
	{
		WidgetController->OnLocalizationPathSelected(*SelectedLocPath);
	}
}

void SELTEditorWidget::SetLocalizationPath(const FString& Path)
{
	TSharedPtr<FString>* FoundPath = LocPathsList.FindByPredicate([&Path](const TSharedPtr<FString>& Item) -> bool
	{
		return *Item == Path;
	});
	if (FoundPath)
	{
		SelectedLocPath = *FoundPath;
	}
}

void SELTEditorWidget::FillLocalizationName(const FString& LocName)
{
	CurrentLocName = LocName;
}

void SELTEditorWidget::FillAvailableLangs(const TArray<FString>& Langs)
{
	AvailableLangs = FString::Join(Langs, TEXT(", "));

	PreviewsAvailables.Empty();
	for (const FString& Lang : Langs)
	{
		PreviewsAvailables.Add(MakeShared<FString>(Lang));
	}
	SelectedPreviewLang = PreviewsAvailables.Num() > 0 ? PreviewsAvailables[0] : nullptr;
	if (WidgetController.IsValid())
	{
		WidgetController->OnLocalizationPreviewLangChanged(*SelectedPreviewLang);
	}

	LanguageOverridesAvailable.Empty();
	for (const FString& Lang : Langs)
	{
		LanguageOverridesAvailable.Add(MakeShared<FString>(Lang));
	}
	SelectedLanguageOverride = LanguageOverridesAvailable.Num() > 0 ? LanguageOverridesAvailable[0] : nullptr;
	if (WidgetController.IsValid())
	{
		WidgetController->OnLocalizationOnFirstRunLangChanged(*SelectedLanguageOverride);
	}
}

void SELTEditorWidget::FillAvailableLangsInLocFile(const TArray<FString>& Langs)
{
	AvailableLangsInLocFile = FString::Join(Langs, TEXT(", "));
}

void SELTEditorWidget::FillCSVPath(const TArray<FString>& CSVPaths)
{
	CSVFiles = FString::Join(CSVPaths, TEXT("\n"));
}

void SELTEditorWidget::SetLocalizationPreview(bool LocalizationPreview)
{
	bIsLocalisationPreviewEnabled_Chkbox = LocalizationPreview;
}

void SELTEditorWidget::SetLocalizationPreviewLang(const FString& PreviewLang)
{
	TSharedPtr<FString>* FoundPreviewLang = PreviewsAvailables.FindByPredicate([&PreviewLang](const TSharedPtr<FString>& Item) -> bool
	{
		return *Item == PreviewLang;
	});
	if (FoundPreviewLang)
	{
		SelectedPreviewLang = *FoundPreviewLang;
	}
}

void SELTEditorWidget::SetReimportAtEditorStartup(bool bReimportAtEditorStartup)
{
	bReimportAtEditorStartup_Chkbox = bReimportAtEditorStartup;
}

void SELTEditorWidget::SetManuallySetLastUsedLanguage(bool bManuallySetLastUsedLanguage)
{
	bManuallySetLastLanguage_Chkbox = bManuallySetLastUsedLanguage;
}

void SELTEditorWidget::SetLocalizationOnFirstRun(bool bLocalizationOnFirstRun)
{
	bOverrideLanguageOnStartup_Chkbox = bLocalizationOnFirstRun;
}

void SELTEditorWidget::SetLocalizationOnFirstRunLang(const FString& OnFirstRunLang)
{
	TSharedPtr<FString>* FoundLanguageOverride = LanguageOverridesAvailable.FindByPredicate([&OnFirstRunLang](const TSharedPtr<FString>& Item) -> bool
	{
		return *Item == OnFirstRunLang;
	});
	if (FoundLanguageOverride)
	{
		SelectedLanguageOverride = *FoundLanguageOverride;
	}
}

void SELTEditorWidget::SetGlobalNamespace(const FString& GlobalNamespace)
{
	GlobalNamespaceValue = GlobalNamespace;
}

void SELTEditorWidget::SetSeparator(const FString& Separator)
{
	SeparatorValue = Separator;
}

void SELTEditorWidget::SetFallbackWhenEmpty(const FString& FallbackWhenEmpty)
{
	TSharedPtr<FString>* FoundFallbackWhenEmpty = FallbackWhenEmptyAvailable.FindByPredicate([&FallbackWhenEmpty](const TSharedPtr<FString>& Item) -> bool
	{
		return *Item == FallbackWhenEmpty;
	});
	if (FoundFallbackWhenEmpty)
	{
		SelectedFallbackWhenEmpty = *FoundFallbackWhenEmpty;
	}
}

void SELTEditorWidget::SetLogDebug(bool bLogDebug)
{
	bLogDebug_Chkbox = bLogDebug;
}

void SELTEditorWidget::SetPreviewInUI(bool bPreviewInUI)
{
	bPreviewInUI_Chkbox = bPreviewInUI;
}

ELTEDITOR_PRAGMA_ENABLE_OPTIMIZATION