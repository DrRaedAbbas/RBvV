// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#include "ablAbilityUtilities.h"

#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/Pawn.h"
#include "InputCoreTypes.h"
#include "UObject/Class.h"
#include "UObject/Package.h"
#include "UObject/UObjectGlobals.h"
#include "UnrealEngine.h"

#define LOCTEXT_NAMESPACE "AblLogHelper"

const FString FAbleLogHelper::GetResultEnumAsString(EAblAbilityStartResult Result)
{
	if (UEnum* EnumClass = FindObject<UEnum>(ANY_PACKAGE, TEXT("EAblAbilityStartResult"), true))
	{
#if WITH_EDITOR
		return EnumClass->GetDisplayNameTextByValue(static_cast<int32>(Result)).ToString();
#else
		return EnumClass->GetNameStringByValue(static_cast<int32>(Result));
#endif
	}

	return NSLOCTEXT("Invalid", "Invalid", "Invalid").ToString();
}

const FString FAbleLogHelper::GetTaskResultEnumAsString(EAblAbilityTaskResult Result)
{
	if (UEnum* EnumClass = FindObject<UEnum>(ANY_PACKAGE, TEXT("EAblAbilityTaskResult"), true))
	{
#if WITH_EDITOR
		return EnumClass->GetDisplayNameTextByValue(static_cast<int32>(Result)).ToString();
#else
		return EnumClass->GetNameStringByValue(static_cast<int32>(Result));
#endif
	}

	return NSLOCTEXT("Invalid", "Invalid", "Invalid").ToString();
}

const FString FAbleLogHelper::GetTargetTargetEnumAsString(EAblAbilityTargetType Result)
{
	if (UEnum* EnumClass = FindObject<UEnum>(ANY_PACKAGE, TEXT("EAblAbilityTargetType"), true))
	{
#if WITH_EDITOR
		return EnumClass->GetDisplayNameTextByValue(static_cast<int32>(Result)).ToString();
#else
		return EnumClass->GetNameStringByValue(static_cast<int32>(Result));
#endif
	}

	return NSLOCTEXT("Invalid", "Invalid", "Invalid").ToString();
}

const FString FAbleLogHelper::GetCollisionResponseEnumAsString(ECollisionResponse Response)
{
	if (UEnum* EnumClass = FindObject<UEnum>(ANY_PACKAGE, TEXT("ECollisionResponse"), true))
	{
#if WITH_EDITOR
		return EnumClass->GetDisplayNameTextByValue(static_cast<int32>(Response)).ToString();
#else
		return EnumClass->GetNameStringByValue(static_cast<int32>(Response));
#endif
	}

	return NSLOCTEXT("Invalid", "Invalid", "Invalid").ToString();
}

const FString FAbleLogHelper::GetCollisionChannelEnumAsString(ECollisionChannel Channel)
{
	if (UEnum* EnumClass = FindObject<UEnum>(ANY_PACKAGE, TEXT("ECollisionChannel"), true))
	{
#if WITH_EDITOR
		return EnumClass->GetDisplayNameTextByValue(static_cast<int32>(Channel)).ToString();
#else
		return EnumClass->GetNameStringByValue(static_cast<int32>(Channel));
#endif
	}

	return NSLOCTEXT("Invalid", "Invalid", "Invalid").ToString();
}

const FString FAbleLogHelper::GetWorldName(const UWorld* World)
{
    // YANKED from SceneOutliner::GetWorldDescription

    FText PostFix;

    const FWorldContext* WorldContext = nullptr;
    for (const FWorldContext& Context : GEngine->GetWorldContexts())
    {
        if (Context.World() == World)
        {
            WorldContext = &Context;
            break;
        }
    }

    if (World->WorldType == EWorldType::PIE)
    {
        switch (World->GetNetMode())
        {
        case NM_Client:
            if (World)
            {
                PostFix = FText::Format(LOCTEXT("ClientPostfixFormat", "(Client {0})"), FText::AsNumber(WorldContext->PIEInstance - 1));
            }
            else
            {
                PostFix = LOCTEXT("ClientPostfix", "(Client)");
            }
            break;
        case NM_DedicatedServer:
        case NM_ListenServer:
            PostFix = LOCTEXT("ServerPostfix", "(Server)");
            break;
        case NM_Standalone:
            PostFix = LOCTEXT("PlayInEditorPostfix", "(Play In Editor)");
            break;
        }
    }
    else if (World->WorldType == EWorldType::Editor)
    {
        PostFix = LOCTEXT("EditorPostfix", "(Editor)");
    }

    return FText::Format(LOCTEXT("WorldFormat", "{0} {1}"), FText::FromString(World->GetFName().GetPlainNameString()), PostFix).ToString();
}

const TArray<FKey> FAblAbilityUtilities::GetKeysForInputAction(const FName& InputAction)
{
	TArray<FKey> ReturnValue;
	const UInputSettings* InputSettings = GetDefault<UInputSettings>();

	if (InputSettings)
	{
		const FName LocalCopy = InputAction;
		for (const FInputActionKeyMapping& ActionMapping : InputSettings->GetActionMappings())
		{
			if (ActionMapping.ActionName == InputAction)
			{
				ReturnValue.Add(ActionMapping.Key);
			}
		}
	}

	return ReturnValue;
}

UBlackboardComponent* FAblAbilityUtilities::GetBlackboard(AActor* Target)
{
	UBlackboardComponent* BlackboardComp = nullptr;

	if (Target != nullptr)
	{
		APawn* TargetPawn = Cast<APawn>(Target);
		if (TargetPawn && TargetPawn->GetController())
		{
			BlackboardComp = TargetPawn->GetController()->FindComponentByClass<UBlackboardComponent>();
		}

		if (BlackboardComp == nullptr)
		{
			BlackboardComp = Target->FindComponentByClass<UBlackboardComponent>();
		}
	}

	return BlackboardComp;
}

#undef LOCTEXT_NAMESPACE