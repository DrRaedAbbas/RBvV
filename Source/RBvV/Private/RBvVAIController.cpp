// Fill out your copyright notice in the Description page of Project Settings.


#include "RBvVAIController.h"
#include "RBVVCharacter.h"
#include "RBvVPlayerController.h"

ARBvVAIController::ARBvVAIController()
{
}

void ARBvVAIController::BeginPlay()
{
    Super::BeginPlay();
}

//void ARBvVAIController::SetTeamID(uint8 NewTeamID)
//{
//    SetGenericTeamId(NewTeamID);
//}
void ARBvVAIController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{	
    if (TeamID != NewTeamID)
    {
        TeamID = NewTeamID;
    }
}

//void ARBvVAIController::GetTeamID()
//{
//	GetGenericTeamId();
//}
FGenericTeamId ARBvVAIController::GetGenericTeamId() const
{
	return TeamID;
}

ETeamAttitude::Type ARBvVAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (const ARBVVCharacter* FoundCharacter = Cast<ARBVVCharacter>(&Other))
	{
        
		if(ARBvVPlayerController* OtherController = Cast<ARBvVPlayerController>(FoundCharacter->GetController()))
		{
            FGenericTeamId OtherID = FGenericTeamId(OtherController->CharacterTeamID);
            if (OtherID == 10) return ETeamAttitude::Neutral;
            if (OtherID == TeamID) return ETeamAttitude::Friendly;
		}
		
		if (ARBvVAIController* OtherController = Cast<ARBvVAIController>(FoundCharacter->GetController()))
		{
			FGenericTeamId OtherID = FGenericTeamId(OtherController->TeamID);
			if (OtherID == 10) return ETeamAttitude::Neutral;
			if (OtherID == TeamID) return ETeamAttitude::Friendly;
		}
	}
 
    return ETeamAttitude::Hostile;
}
