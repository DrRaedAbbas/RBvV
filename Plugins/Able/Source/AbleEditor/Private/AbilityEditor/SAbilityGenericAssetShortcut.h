#pragma once

#include "AssetData.h"
#include "AssetThumbnail.h"
#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/SOverlay.h"

class FAblAssetShortcutParams
{
public:
	DECLARE_DELEGATE_OneParam(FOnAssetSelectedCallback, const FAssetData& /*AssetData*/);
	DECLARE_DELEGATE(FOnAssetClicked);
	DECLARE_DELEGATE_RetVal(FAssetData, FOnGetInitialAsset);

	FAblAssetShortcutParams();
	FAblAssetShortcutParams(const UClass* InRequiredParentClass, const FText& InDisplayName, const FSlateBrush* InBrush, const FSlateColor& InColor);
	~FAblAssetShortcutParams();

	bool IsAssetCompatible(const FAssetData& InAssetData) const;

	FText ShortcutDisplayName;

	const FSlateBrush* ShortcutIconBrush;

	FSlateColor ShortcutIconColor;

	const UClass* RequiredParentClass;

	FOnAssetSelectedCallback AssetCallback;
	FOnAssetClicked AssetClickCallback;
	FOnGetInitialAsset InitialAssetCallback;
};

class SAblAssetFamilyShortcutBar : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAblAssetFamilyShortcutBar)
	{}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<class FWorkflowCentricApplication>& InHostingApp, const TArray<TSharedPtr<FAblAssetShortcutParams>>& InParams);

private:
	/** The thumbnail pool for displaying asset shortcuts */
	TSharedPtr<class FAssetThumbnailPool> ThumbnailPool;
};

//#define USE_OLD_ABLE_ABILITY_SHORTCUT
#ifdef USE_OLD_ABLE_ABILITY_SHORTCUT
// This is just a copy/paste of the SAssetShortcut with a few small differences (we simply use a delegate when the asset changes, rather than opening the associated editor, no asset family is required, etc).
// Sorry for the straight copy/paste, love you guys. <3
class SAblGenericAssetShortcut : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAblGenericAssetShortcut)
	{}

	SLATE_END_ARGS()

	DECLARE_DELEGATE_OneParam(FOnAssetSelectedCallback, const FAssetData& /*AssetData*/);
	DECLARE_DELEGATE_RetVal(TSharedRef<SWidget>, FOnAddAdditionalWidgets);

	void Construct(const FArguments& InArgs, const FAssetData& InAssetData, const TSharedRef<FAssetThumbnailPool>& InThumbnailPool, const FText& Label, const FOnAddAdditionalWidgets& extraWidgets);

	~SAblGenericAssetShortcut();

	void HandleOpenAssetShortcut(ECheckBoxState InState);

	FText GetAssetText() const;
	const FSlateBrush* GetAssetIcon() const;
	FSlateColor GetAssetTint() const;
	FSlateColor GetAssetTextColor() const;

	ECheckBoxState GetCheckState() const;

	TSharedRef<SWidget> HandleGetMenuContent();

	void HandleAssetSelectedFromPicker(const struct FAssetData& InAssetData);

	bool HandleFilterAsset(const struct FAssetData& InAssetData);
	EVisibility GetComboButtonVisibility() const
	EVisibility GetComboVisibility() const;

	void HandleFilesLoaded();
	void HandleAssetRemoved(const FAssetData& InAssetData);
	void HandleAssetRenamed(const FAssetData& InAssetData, const FString& InOldObjectPath);
	void HandleAssetAdded(const FAssetData& InAssetData);

	void HandleShowInContentBrowser();
	void HandleAssetOpened(UObject* InAsset);

	EVisibility GetThumbnailVisibility() const;
	EVisibility GetSmallThumbnailVisibility() const;
	const FSlateBrush* GetDirtyImage() const;

	void RefreshAsset();

	void RegenerateThumbnail();
	EActiveTimerReturnType HandleRefreshDirtyState(double InCurrentTime, float InDeltaTime);
	FText GetButtonTooltip() const;

	FOnAssetSelectedCallback& OnAssetSelectedCallback() { return mCallback; }
	void SetHostingApp(const TWeakPtr<class FWorkflowCentricApplication> InHostingApp) { HostingApp = InHostingApp; }
	void SetFilterData(const TArray<FName>& InAssetFilter) { AssetFilter = InAssetFilter; }
	void SetClassFilter(const TArray<UClass*>& InClassFilter) { ClassFilter = InClassFilter; }
private:
	FOnAssetSelectedCallback mCallback;

	/** The current asset data for this widget */
	FAssetData AssetData;

	TArray<FName> AssetFilter;
	TArray<UClass*> ClassFilter;

	/** Cache the package of the object for checking dirty state */
	TWeakObjectPtr<UPackage> AssetPackage;

	/** Timer handle used to updating dirty state */
	TSharedPtr<class FActiveTimerHandle> DirtyStateTimerHandle;

	/** Our asset thumbnails */
	TSharedPtr<FAssetThumbnail> AssetThumbnail;
	TSharedPtr<FAssetThumbnail> AssetThumbnailSmall;

	/** Thumbnail widget containers */
	TSharedPtr<SBox> ThumbnailBox;
	TSharedPtr<SBox> ThumbnailSmallBox;

	/** The asset editor we are embedded in */
	TWeakPtr<class FWorkflowCentricApplication> HostingApp;

	/** Thumbnail pool */
	TWeakPtr<FAssetThumbnailPool> ThumbnailPoolPtr;

	/** Check box */
	TSharedPtr<SCheckBox> CheckBox;

	/** Cached dirty brush */
	const FSlateBrush* AssetDirtyBrush;

	/** Whether there are multiple (>1) of this asset type in existence */
	bool bMultipleAssetsExist;

	/** Cache the package's dirty state */
	bool bPackageDirty;
};
#endif