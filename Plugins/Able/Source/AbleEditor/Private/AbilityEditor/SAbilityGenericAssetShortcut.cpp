#include "AbilityEditor/SAbilityGenericAssetShortcut.h"

#include "AssetRegistryModule.h"
#include "ContentBrowserModule.h"
#include "Editor.h"
#include "EditorStyleSet.h"
#include "Engine/Blueprint.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Commands/UIAction.h"
#include "Framework/MultiBox/MultiBoxDefs.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "GameFramework/Pawn.h"
#include "IContentBrowserSingleton.h"
#include "Modules/ModuleManager.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateTypes.h"
#include "Styling/ToolBarStyle.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Textures/SlateIcon.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"

#define LOCTEXT_NAMESPACE "AblAbilityEditor"

namespace GenericAssetShortcutConstants
{
	const int32 ThumbnailSize = 40;
	const int32 ThumbnailSizeSmall = 16;
}

class SAblAssetShortcut : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAblAssetShortcut)
	{}
	
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<class FWorkflowCentricApplication>& InHostingApp, const TSharedPtr<FAblAssetShortcutParams>& InParams, const TSharedRef<FAssetThumbnailPool>& InThumbnailPool)
	{
		ShortcutParams = InParams;
		HostingApp = InHostingApp;

		AssetDirtyBrush = FAppStyle::Get().GetBrush("Icons.DirtyBadge");

		const FToolBarStyle& ToolBarStyle = FEditorStyle::Get().GetWidgetStyle<FToolBarStyle>("ToolBar");

		ChildSlot
			[
				SNew(SHorizontalBox)

		// This is the left half of the button / combo pair for when there are multiple options
		+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			[
				SAssignNew(CheckBox, SCheckBox)
				.Style(FAppStyle::Get(), "SegmentedCombo.Left")
				.OnCheckStateChanged(this, &SAblAssetShortcut::HandleAssetButtonClicked)
				.IsChecked(this, &SAblAssetShortcut::GetCheckState)
				.Visibility(this, &SAblAssetShortcut::GetComboButtonVisibility)
				.ToolTipText(this, &SAblAssetShortcut::GetButtonTooltip)
				.Padding(0.0)
				[
					SNew(SOverlay)

					+ SOverlay::Slot()
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Center)
					.Padding(FMargin(16.f, 4.f))
					[
						SNew(SImage)
						.ColorAndOpacity(this, &SAblAssetShortcut::GetAssetColor)
						.Image(this, &SAblAssetShortcut::GetAssetIcon)
					]
				]
			]
		+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.AutoWidth()
			[
				SNew(SSeparator)
				.Visibility(this, &SAblAssetShortcut::GetComboVisibility)
			.Thickness(1.0)
			.Orientation(EOrientation::Orient_Vertical)
			]
		+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.AutoWidth()
			[
				SNew(SComboButton)
				.Visibility(this, &SAblAssetShortcut::GetComboVisibility)
			.ContentPadding(FMargin(7.f, 0.f))
			.ForegroundColor(FSlateColor::UseForeground())
			.ComboButtonStyle(&FAppStyle::Get(), "SegmentedCombo.Right")
			.OnGetMenuContent(this, &SAblAssetShortcut::HandleGetMenuContent)
			.ToolTipText(LOCTEXT("AssetComboTooltip", "Find other assets of this type and perform asset operations.\nShift-Click to open in new window."))
			]
			];

		EnableToolTipForceField(true);
	}

	~SAblAssetShortcut()
	{

	}

	const FText& GetAssetName() const
	{
		return ShortcutParams->ShortcutDisplayName;
	}

	const FSlateBrush* GetAssetIcon() const
	{
		return ShortcutParams->ShortcutIconBrush;
	}

	FSlateColor GetAssetColor() const
	{
		return ShortcutParams->ShortcutIconColor;
	}

	ECheckBoxState GetCheckState() const
	{
		return ECheckBoxState::Unchecked;
	}

	FSlateColor GetAssetTextColor() const
	{
		static const FName InvertedForeground("InvertedForeground");
		return GetCheckState() == ECheckBoxState::Checked || CheckBox->IsHovered() ? FEditorStyle::GetSlateColor(InvertedForeground) : FSlateColor::UseForeground();
	}

	void HandleAssetButtonClicked(ECheckBoxState InState)
	{
		ShortcutParams->AssetClickCallback.ExecuteIfBound();
	}

	TSharedRef<SWidget> HandleGetMenuContent()
	{
		FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

		const bool bInShouldCloseWindowAfterMenuSelection = true;
		FMenuBuilder MenuBuilder(bInShouldCloseWindowAfterMenuSelection, nullptr);

		//MenuBuilder.BeginSection("AssetActions", LOCTEXT("AssetActionsSection", "Asset Actions"));
		//{
		//	MenuBuilder.AddMenuEntry(
		//		LOCTEXT("ShowInContentBrowser", "Show In Content Browser"),
		//		LOCTEXT("ShowInContentBrowser_ToolTip", "Show this asset in the content browser."),
		//		FSlateIcon(FEditorStyle::GetStyleSetName(), "Icons.Search"),
		//		FUIAction(FExecuteAction::CreateSP(this, &SAblAssetShortcut::HandleShowInContentBrowser)));
		//}
		//MenuBuilder.EndSection();

		FAssetData InitialData = ShortcutParams->InitialAssetCallback.IsBound() ? ShortcutParams->InitialAssetCallback.Execute() : FAssetData();


		MenuBuilder.BeginSection("AssetSelection", LOCTEXT("AssetSelectionSection", "Select Asset"));
		{
			FAssetPickerConfig AssetPickerConfig;

			AssetPickerConfig.Filter.ClassNames.Add(UBlueprint::StaticClass()->GetFName());
			AssetPickerConfig.Filter.bRecursiveClasses = true;
			AssetPickerConfig.SelectionMode = ESelectionMode::SingleToggle;
			AssetPickerConfig.OnAssetSelected = FOnAssetSelected::CreateSP(this, &SAblAssetShortcut::HandleAssetSelectedFromPicker);
			AssetPickerConfig.OnShouldFilterAsset = FOnShouldFilterAsset::CreateSP(this, &SAblAssetShortcut::ShouldFilterAsset);
			AssetPickerConfig.bAllowNullSelection = false;
			AssetPickerConfig.ThumbnailLabel = EThumbnailLabel::ClassName;
			AssetPickerConfig.InitialAssetViewType = EAssetViewType::List;
			AssetPickerConfig.InitialAssetSelection = InitialData;

			MenuBuilder.AddWidget(
				SNew(SBox)
				.WidthOverride(300)
				.HeightOverride(600)
				[
					ContentBrowserModule.Get().CreateAssetPicker(AssetPickerConfig)
				],
				FText(), true);
		}
		MenuBuilder.EndSection();


		return MenuBuilder.MakeWidget();
	}

	void HandleAssetSelectedFromPicker(const struct FAssetData& InAssetData)
	{
		ShortcutParams->AssetCallback.ExecuteIfBound(InAssetData);
	}

	bool ShouldFilterAsset(const struct FAssetData& InAssetData)
	{
		return !ShortcutParams->IsAssetCompatible(InAssetData);
	}

	EVisibility GetComboButtonVisibility() const
	{
		return EVisibility::Visible;
	}

	EVisibility GetComboVisibility() const
	{
		return EVisibility::Visible;
	}

	FText GetButtonTooltip() const
	{
		return ShortcutParams->ShortcutDisplayName;
	}

private:
	/** The asset family we are working with */
	TSharedPtr<FAblAssetShortcutParams> ShortcutParams;

	/** The asset editor we are embedded in */
	TWeakPtr<class FWorkflowCentricApplication> HostingApp;

	/** Check box */
	TSharedPtr<SCheckBox> CheckBox;

	/** Cached dirty brush */
	const FSlateBrush* AssetDirtyBrush;
};

void SAblAssetFamilyShortcutBar::Construct(const FArguments& InArgs, const TSharedRef<class FWorkflowCentricApplication>& InHostingApp, const TArray<TSharedPtr<FAblAssetShortcutParams>>& InParams)
{
	ThumbnailPool = MakeShareable(new FAssetThumbnailPool(16, false));

	TSharedRef<SHorizontalBox> HorizontalBox = SNew(SHorizontalBox);

	for(const TSharedPtr<FAblAssetShortcutParams>& Param : InParams)
	{
		HorizontalBox->AddSlot()
			.AutoWidth()
			.Padding(0.0f, 4.0f, 16.0f, 4.0f)
			[
				SNew(SAblAssetShortcut, InHostingApp, Param, ThumbnailPool.ToSharedRef())
			];
	}

	ChildSlot
		[
			HorizontalBox
		];
}

#ifdef USE_OLD_ABLE_ABILITY_SHORTCUT 
void SAblGenericAssetShortcut::Construct(const FArguments& InArgs, const FAssetData& InAssetData, const TSharedRef<FAssetThumbnailPool>& InThumbnailPool, const FText& Label, const FOnAddAdditionalWidgets& extraWidgets)
{
	AssetData = InAssetData;
	ThumbnailPoolPtr = InThumbnailPool;
	bPackageDirty = false;

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	AssetRegistryModule.Get().OnAssetRenamed().AddSP(this, &SAblGenericAssetShortcut::HandleAssetRenamed);

	if (GEditor)
	{
		GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OnAssetEditorRequestedOpen().AddSP(this, &SAblGenericAssetShortcut::HandleAssetOpened);
	}

	AssetThumbnail = MakeShareable(new FAssetThumbnail(InAssetData, GenericAssetShortcutConstants::ThumbnailSize, GenericAssetShortcutConstants::ThumbnailSize, InThumbnailPool));
	AssetThumbnailSmall = MakeShareable(new FAssetThumbnail(InAssetData, GenericAssetShortcutConstants::ThumbnailSizeSmall, GenericAssetShortcutConstants::ThumbnailSizeSmall, InThumbnailPool));

	bMultipleAssetsExist = true; // Needed?
	AssetDirtyBrush = FEditorStyle::GetBrush("ContentBrowser.ContentDirty");

	TSharedPtr<SWidget> extraWidgetResult = nullptr;

	if (extraWidgets.IsBound())
	{
		extraWidgetResult = extraWidgets.Execute();
	}

	TSharedRef<SHorizontalBox> MainWidget = SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		[
			SAssignNew(CheckBox, SCheckBox)
			.Style(FEditorStyle::Get(), "ToolBar.ToggleButton")
		.ForegroundColor(FSlateColor::UseForeground())
		.Padding(0.0f)
		.OnCheckStateChanged(this, &SAblGenericAssetShortcut::HandleOpenAssetShortcut)
		.IsChecked(this, &SAblGenericAssetShortcut::GetCheckState)
		.Visibility(this, &SAblGenericAssetShortcut::GetButtonVisibility)
		.ToolTipText(this, &SAblGenericAssetShortcut::GetButtonTooltip)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.AutoWidth()
		[
			SNew(SBorder)
			.Padding(4.0f)
		.BorderImage(FEditorStyle::GetBrush("PropertyEditor.AssetThumbnailShadow"))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		[
			SAssignNew(ThumbnailBox, SBox)
			.WidthOverride(GenericAssetShortcutConstants::ThumbnailSize)
		.HeightOverride(GenericAssetShortcutConstants::ThumbnailSize)
		.Visibility(this, &SAblGenericAssetShortcut::GetThumbnailVisibility)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
		[
			AssetThumbnail->MakeThumbnailWidget()
		]
	+ SOverlay::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Bottom)
		[
			SNew(SImage)
			.Image(this, &SAblGenericAssetShortcut::GetDirtyImage)
		]
		]
		]
	+ SHorizontalBox::Slot()
		[
			SAssignNew(ThumbnailSmallBox, SBox)
			.WidthOverride(GenericAssetShortcutConstants::ThumbnailSizeSmall)
		.HeightOverride(GenericAssetShortcutConstants::ThumbnailSizeSmall)
		.Visibility(this, &SAblGenericAssetShortcut::GetSmallThumbnailVisibility)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
		[
			AssetThumbnailSmall->MakeThumbnailWidget()
		]
	+ SOverlay::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Bottom)
		[
			SNew(SImage)
			.Image(this, &SAblGenericAssetShortcut::GetDirtyImage)
		]
		]
		]
		]
		]
	+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.AutoWidth()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(4.0f)
			[
				SNew(STextBlock)
				.Text(this, &SAblGenericAssetShortcut::GetAssetText)
				.TextStyle(FEditorStyle::Get(), "Toolbar.Label")
				.ShadowOffset(FVector2D::UnitVector)
			]
		]
		]
		]
	+ SHorizontalBox::Slot()
		.VAlign(VAlign_Fill)
		.AutoWidth()
		.Padding(2.0f, 0.0f, 0.0f, 0.0f)
		[
			SNew(SComboButton)
			.Visibility(this, &SAblGenericAssetShortcut::GetComboVisibility)
			.ContentPadding(0)
			.ForegroundColor(FSlateColor::UseForeground())
			.ButtonStyle(FEditorStyle::Get(), "Toolbar.Button")
			.OnGetMenuContent(this, &SAblGenericAssetShortcut::HandleGetMenuContent)
			.ToolTipText(LOCTEXT("AssetComboTooltip", "Find other assets of this type and perform asset operations.Shift-Click to open in new window."))
		];

	if (extraWidgetResult.IsValid())
	{
		MainWidget->AddSlot()
		.AutoWidth()
			[
				extraWidgetResult->AsShared()
			];
	}

	ChildSlot
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			[
				MainWidget
			]

			+ SVerticalBox::Slot()
			.HAlign(HAlign_Center)
			.AutoHeight()
				[
					SNew(STextBlock)
					.Text(Label)
					.TextStyle(FEditorStyle::Get(), "Toolbar.Label")
					.ShadowOffset(FVector2D::UnitVector)
				]
		];

	EnableToolTipForceField(true);

	DirtyStateTimerHandle = RegisterActiveTimer(1.0f / 10.0f, FWidgetActiveTimerDelegate::CreateSP(this, &SAblGenericAssetShortcut::HandleRefreshDirtyState));*/
}

SAblGenericAssetShortcut::~SAblGenericAssetShortcut()
{
	if (FModuleManager::Get().IsModuleLoaded(TEXT("AssetRegistry")))
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::GetModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		AssetRegistryModule.Get().OnAssetRenamed().RemoveAll(this);
	}

	if (GEditor)
	{
		GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OnAssetEditorRequestedOpen().RemoveAll(this);
	}

	UnRegisterActiveTimer(DirtyStateTimerHandle.ToSharedRef());
}

void SAblGenericAssetShortcut::HandleOpenAssetShortcut(ECheckBoxState InState)
{
	if (AssetData.IsValid() && mCallback.IsBound())
	{
		mCallback.Execute(AssetData);
	}
}

FText SAblGenericAssetShortcut::GetAssetText() const
{
	if (AssetData.IsValid())
	{
		return FText::FromName(AssetData.AssetName);
	}

	return LOCTEXT("EMPTY_STRING", "");
}

ECheckBoxState SAblGenericAssetShortcut::GetCheckState() const
{
	const TArray<UObject*>* Objects = HostingApp.Pin()->GetObjectsCurrentlyBeingEdited();
	if (Objects != nullptr)
	{
		for (UObject* Object : *Objects)
		{
			if (FAssetData(Object) == AssetData)
			{
				return ECheckBoxState::Checked;
			}
		}
	}
	return ECheckBoxState::Unchecked;
}

FSlateColor SAblGenericAssetShortcut::GetAssetTextColor() const
{
	static const FName InvertedForeground("InvertedForeground");
	return GetCheckState() == ECheckBoxState::Checked || CheckBox->IsHovered() ? FEditorStyle::GetSlateColor(InvertedForeground) : FSlateColor::UseForeground();
}

TSharedRef<SWidget> SAblGenericAssetShortcut::HandleGetMenuContent()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

	const bool bInShouldCloseWindowAfterMenuSelection = true;
	FMenuBuilder MenuBuilder(bInShouldCloseWindowAfterMenuSelection, nullptr);

	MenuBuilder.BeginSection("AssetActions", LOCTEXT("AssetActionsSection", "Asset Actions"));
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("ShowInContentBrowser", "Show In Content Browser"),
			LOCTEXT("ShowInContentBrowser_ToolTip", "Show this asset in the content browser."),
			FSlateIcon(FEditorStyle::GetStyleSetName(), "PropertyWindow.Button_Browse"),
			FUIAction(FExecuteAction::CreateSP(this, &SAblGenericAssetShortcut::HandleShowInContentBrowser)));
	}
	MenuBuilder.EndSection();

	if (bMultipleAssetsExist)
	{
		MenuBuilder.BeginSection("AssetSelection", LOCTEXT("AssetSelectionSection", "Select Asset"));
		{
			FAssetPickerConfig AssetPickerConfig;
			AssetPickerConfig.Filter.ClassNames.Append(UBlueprint::StaticClass()->GetFName());
			AssetPickerConfig.Filter.bRecursiveClasses = true;
			AssetPickerConfig.SelectionMode = ESelectionMode::SingleToggle;
			AssetPickerConfig.OnAssetSelected = FOnAssetSelected::CreateSP(this, &SAblGenericAssetShortcut::HandleAssetSelectedFromPicker);
			AssetPickerConfig.OnShouldFilterAsset = FOnShouldFilterAsset::CreateSP(this, &SAblGenericAssetShortcut::HandleFilterAsset);
			AssetPickerConfig.bAllowNullSelection = true;
			AssetPickerConfig.bCanShowClasses = false;
			AssetPickerConfig.ThumbnailLabel = EThumbnailLabel::ClassName;
			AssetPickerConfig.InitialAssetViewType = EAssetViewType::List;
			AssetPickerConfig.InitialAssetSelection = AssetData;
			AssetPickerConfig.bPreloadAssetsForContextMenu = true;

			MenuBuilder.AddWidget(
				SNew(SBox)
				.WidthOverride(300)
				.HeightOverride(600)
				[
					ContentBrowserModule.Get().CreateAssetPicker(AssetPickerConfig)
				],
				FText(), true);
		}
		MenuBuilder.EndSection();
	}

	return MenuBuilder.MakeWidget();
}

void SAblGenericAssetShortcut::HandleAssetSelectedFromPicker(const FAssetData& InAssetData)
{
	if (mCallback.IsBound())
	{
		FSlateApplication::Get().DismissAllMenus();
		if (InAssetData.IsValid())
		{
			mCallback.Execute(InAssetData);
		}
		else if (AssetData.IsValid())
		{
			mCallback.Execute(AssetData);
		}
	}
}

bool SAblGenericAssetShortcut::HandleFilterAsset(const FAssetData& InAssetData)
{
	if (InAssetData.AssetClass == UBlueprint::StaticClass()->GetFName())
	{
		if (UBlueprint* LoadedBlueprint = Cast<UBlueprint>(InAssetData.GetAsset()))
		{
			if (LoadedBlueprint->GeneratedClass)
			{
				return !LoadedBlueprint->GeneratedClass->IsChildOf(APawn::StaticClass());
			}
		}
	}

	return !AssetFilter.Contains(InAssetData.AssetClass);
}

EVisibility SAblGenericAssetShortcut::GetButtonVisibility() const
{
	return EVisibility::Visible;
}

EVisibility SAblGenericAssetShortcut::GetComboVisibility() const
{
	return EVisibility::Visible;
}

void SAblGenericAssetShortcut::HandleFilesLoaded()
{
	/* Don't care.*/
}

void SAblGenericAssetShortcut::HandleAssetRemoved(const FAssetData& InAssetData)
{
	/* Don't care.*/
}

void SAblGenericAssetShortcut::HandleAssetRenamed(const FAssetData& InAssetData, const FString& InOldObjectPath)
{
	if (InOldObjectPath == AssetData.ObjectPath.ToString())
	{
		AssetData = InAssetData;

		RegenerateThumbnail();
	}
}

void SAblGenericAssetShortcut::HandleAssetAdded(const FAssetData& InAssetData)
{
	/* Don't care.*/
}

void SAblGenericAssetShortcut::HandleShowInContentBrowser()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	TArray<FAssetData> Assets;
	Assets.Add(AssetData);
	ContentBrowserModule.Get().SyncBrowserToAssets(Assets);
}

void SAblGenericAssetShortcut::HandleAssetOpened(UObject* InAsset)
{
	RefreshAsset();
}

EVisibility SAblGenericAssetShortcut::GetThumbnailVisibility() const
{
	return FMultiBoxSettings::UseSmallToolBarIcons.Get() ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility SAblGenericAssetShortcut::GetSmallThumbnailVisibility() const
{
	return FMultiBoxSettings::UseSmallToolBarIcons.Get() ? EVisibility::Visible : EVisibility::Collapsed;
}

const FSlateBrush* SAblGenericAssetShortcut::GetDirtyImage() const
{
	return bPackageDirty ? AssetDirtyBrush : nullptr;
}

void SAblGenericAssetShortcut::RefreshAsset()
{
	// Don't think we care about this?
}

void SAblGenericAssetShortcut::RegenerateThumbnail()
{
	if (AssetData.IsValid())
	{
		AssetThumbnail = MakeShareable(new FAssetThumbnail(AssetData, GenericAssetShortcutConstants::ThumbnailSize, GenericAssetShortcutConstants::ThumbnailSize, ThumbnailPoolPtr.Pin()));
		AssetThumbnailSmall = MakeShareable(new FAssetThumbnail(AssetData, GenericAssetShortcutConstants::ThumbnailSizeSmall, GenericAssetShortcutConstants::ThumbnailSizeSmall, ThumbnailPoolPtr.Pin()));

		ThumbnailBox->SetContent(AssetThumbnail->MakeThumbnailWidget());
		ThumbnailSmallBox->SetContent(AssetThumbnailSmall->MakeThumbnailWidget());
	}
}

EActiveTimerReturnType SAblGenericAssetShortcut::HandleRefreshDirtyState(double InCurrentTime, float InDeltaTime)
{
	if (AssetData.IsAssetLoaded())
	{
		if (!AssetPackage.IsValid())
		{
			AssetPackage = AssetData.GetPackage();
		}

		if (AssetPackage.IsValid())
		{
			bPackageDirty = AssetPackage->IsDirty();
		}
	}

	return EActiveTimerReturnType::Continue;
}

FText SAblGenericAssetShortcut::GetButtonTooltip() const
{
	return FText::Format(LOCTEXT("AssetTooltipFormat", "{0}\n{1}"), FText::FromName(AssetData.AssetName), FText::FromString(AssetData.GetFullName()));
}
#endif

FAblAssetShortcutParams::FAblAssetShortcutParams()
: ShortcutDisplayName(LOCTEXT("AblShortcutInvalidName", "INVALID NAME")),
ShortcutIconBrush(),
ShortcutIconColor(FSlateColor::UseForeground()),
RequiredParentClass(nullptr)
{

}

FAblAssetShortcutParams::FAblAssetShortcutParams(const UClass* InRequiredParentClass, const FText& InDisplayName, const FSlateBrush* InBrush, const FSlateColor& InColor)
: ShortcutDisplayName(InDisplayName),
ShortcutIconBrush(InBrush),
ShortcutIconColor(InColor),
RequiredParentClass(InRequiredParentClass)
{

}

FAblAssetShortcutParams::~FAblAssetShortcutParams()
{

}

bool FAblAssetShortcutParams::IsAssetCompatible(const FAssetData& InAssetData) const
{
	if (!RequiredParentClass)
	{
		return false;
	}

	if (InAssetData.AssetClass == UBlueprint::StaticClass()->GetFName())
	{
		if (UBlueprint* LoadedBlueprint = Cast<UBlueprint>(InAssetData.GetAsset()))
		{
			return *(LoadedBlueprint->GeneratedClass) && LoadedBlueprint->GeneratedClass->IsChildOf(RequiredParentClass);
		}
	}
	return false;
}
