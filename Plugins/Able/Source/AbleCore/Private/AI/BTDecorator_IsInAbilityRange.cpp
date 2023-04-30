// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#include "AI/BTDecorator_IsInAbilityRange.h"

#include "ablAbility.h"
#include "ablAbilityComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "VisualLogger/VisualLogger.h"

UBTDecorator_IsInAbilityRange::UBTDecorator_IsInAbilityRange(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Is In Ability Range";

	PointA.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_IsInAbilityRange, PointA), AActor::StaticClass());
	PointA.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_IsInAbilityRange, PointA));

	PointB.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_IsInAbilityRange, PointB), AActor::StaticClass());
	PointB.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_IsInAbilityRange, PointB));

	// Default to using Self Actor
	PointA.SelectedKeyName = FBlackboard::KeySelf;
	AbilityKey.SelectedKeyName = FBlackboard::KeySelf;

	// For now, don't allow users to select any "Abort Observers", because it's currently not supported.
	bAllowAbortNone = false;
	bAllowAbortLowerPri = false;
	bAllowAbortChildNodes = false;
	FlowAbortMode = EBTFlowAbortMode::None;
}

const UAblAbility* UBTDecorator_IsInAbilityRange::GetAbility(const UBlackboardComponent* BlackboardComp) const
{
    if (BlackboardComp == nullptr)
    {
        return nullptr;
    }

    if (AbilityKey.IsSet())
    {
        UClass* AbilityClass = BlackboardComp->GetValueAsClass(AbilityKey.SelectedKeyName);
        if (AbilityClass == nullptr)
        {
            UE_VLOG(BlackboardComp->GetOwner(), LogBehaviorTree, Warning, TEXT("Tree %s, Node %s, NULL Ability from Blackboard( %s ) Key: %s"),
                *GetTreeAsset()->GetFName().ToString(),
                *GetParentNode()->GetNodeName(),
                *BlackboardComp->GetFName().ToString(),
                *AbilityKey.SelectedKeyName.ToString());
        }
        else if (const UAblAbility* AbilityCDO = Cast<UAblAbility>(AbilityClass->GetDefaultObject()))
        {
            return AbilityCDO;
        }
    }
    else if (Ability != nullptr)
    {
        if (const UAblAbility* AbilityCDO = Cast<UAblAbility>(Ability.GetDefaultObject()))
        {
            return AbilityCDO;
        }
    }
    return nullptr;
}

bool UBTDecorator_IsInAbilityRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp == nullptr)
	{
		return false;
	}

    if (const UAblAbility* AbilityCDO = GetAbility(BlackboardComp))
	{
        const float AbilityRange = AbilityCDO->GetRange();
        const float AbilityRangeSqr = AbilityRange * AbilityRange;

        if (AbilityRange < KINDA_SMALL_NUMBER)
        {
            // No range, allow it.
            return true;
        }
		FVector Origin (ForceInitToZero), Destination(ForceInitToZero);

		// Grab Point A
		if (PointA.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
		{
			UObject* KeyValue = BlackboardComp->GetValue<UBlackboardKeyType_Object>(PointA.GetSelectedKeyID());
			AActor* TargetActor = Cast<AActor>(KeyValue);
			if (TargetActor)
			{
				Origin = TargetActor->GetActorLocation();
			}
		}
		else if (PointA.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
		{
			Origin = BlackboardComp->GetValue<UBlackboardKeyType_Vector>(PointA.GetSelectedKeyID());
		}

		// Grab Point B
		if (PointB.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
		{
			UObject* KeyValue = BlackboardComp->GetValue<UBlackboardKeyType_Object>(PointB.GetSelectedKeyID());
			AActor* TargetActor = Cast<AActor>(KeyValue);
			if (TargetActor)
			{
				Destination = TargetActor->GetActorLocation();
			}
		}
		else if (PointB.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
		{
			Destination = BlackboardComp->GetValue<UBlackboardKeyType_Vector>(PointB.GetSelectedKeyID());
		}

		float DistanceSqr = XYDistance ? FVector::DistSquaredXY(Destination, Origin) : FVector::DistSquared(Destination, Origin);

		UE_VLOG(BlackboardComp->GetOwner(), LogBehaviorTree, Verbose, TEXT("Tree %s, Node %s, Checking Ability Range from Blackboard( %s ) PointA ( %s ) PointB ( %s ) Ability( %s ) Range Squared( %.4f )"),
			*GetTreeAsset()->GetFName().ToString(),
			*GetParentNode()->GetNodeName(),
			*BlackboardComp->GetFName().ToString(),
			*AbilityCDO->GetDisplayName(),
			*Origin.ToCompactString(),
			*Destination.ToCompactString(),
			AbilityRangeSqr);

		return DistanceSqr <= AbilityRangeSqr;

	}

	return false;
}

FString UBTDecorator_IsInAbilityRange::GetStaticDescription() const
{
    FString AbilityName = TEXT("NONE");
    if (AbilityKey.IsSet())
	{
        AbilityName = FString::Format(TEXT("BlueprintKey(%s)"), { AbilityKey.SelectedKeyName.ToString() });
	}
    else if (Ability != nullptr)
	{
        AbilityName = FString::Format(TEXT("AbilityClass(%s)"), { Ability->GetDefaultObject()->GetName() });
	}

    return FString::Printf(TEXT("%s: checks if the distance from Point A to Point B are within range of Ability [%s]."), *Super::GetStaticDescription(), *AbilityName);
}

void UBTDecorator_IsInAbilityRange::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		PointA.ResolveSelectedKey(*BBAsset);
		PointB.ResolveSelectedKey(*BBAsset);
		AbilityKey.ResolveSelectedKey(*BBAsset);
	}
}