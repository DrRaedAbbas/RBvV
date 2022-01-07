// Fill out your copyright notice in the Description page of Project Settings.


#include "RBvVPlayerController.h"

#include "RBvVAIController.h"
#include "RBVVCharacter.h"

ARBvVPlayerController::ARBvVPlayerController()
{
}

void ARBvVPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ARBvVPlayerController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (CharacterTeamID != NewTeamID)
	{
		CharacterTeamID = NewTeamID;
	}
}

FGenericTeamId ARBvVPlayerController::GetGenericTeamId() const
{
	return CharacterTeamID;
}

//void ARBvVPlayerController::SetTeamID(uint8 NewTeamID)
//{
//	
//	IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(this);
//
//	if (TeamAgent != nullptr)
//	{
//		TeamAgent->SetGenericTeamId(NewTeamID);
//		CharacterTeamID = GetGenericTeamId();
//	}
//	
////	SetGenericTeamId(NewTeamID);
////	CharacterTeamID = GetGenericTeamId();
////	CharacterTeamID = NewTeamID;
//}

//uint8 ARBvVPlayerController::GetTeamID()
//{
//	return CharacterTeamID;
//}

ETeamAttitude::Type ARBvVPlayerController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (const ARBVVCharacter* FoundCharacter = Cast<ARBVVCharacter>(&Other))
	{	
		if (ARBvVPlayerController* OtherController = Cast<ARBvVPlayerController>(FoundCharacter->GetController()))
		{
			FGenericTeamId OtherID = FGenericTeamId(OtherController->CharacterTeamID);
			if (OtherID == 10) return ETeamAttitude::Neutral;
			if (OtherID == CharacterTeamID) return ETeamAttitude::Friendly;
		}
		
		if (ARBvVAIController* OtherController = Cast<ARBvVAIController>(FoundCharacter->GetController()))
		{
			FGenericTeamId OtherID = FGenericTeamId(OtherController->TeamID);
			if (OtherID == 10) return ETeamAttitude::Neutral;
			if (OtherID == CharacterTeamID) return ETeamAttitude::Friendly;
		}
	}

	return ETeamAttitude::Hostile;
}
