// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"

#include "RBvVPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class RBVV_API ARBvVPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	
	ARBvVPlayerController();
	virtual void BeginPlay() override;
	
	//FGenericTeamId CharacterTeamID;
	UPROPERTY(BlueprintReadOnly, Category = "RBVV")
	uint8 CharacterTeamID;

	/*UFUNCTION(BlueprintCallable, Category = "RBVV")
	void SetTeamID(uint8 NewTeamID);

	UFUNCTION(BlueprintCallable, Category = "RBVV")
	uint8 GetTeamID();*/
	UFUNCTION(BlueprintCallable, Category = "RBVV")
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;

	UFUNCTION(BlueprintCallable, Category = "RBVV")
	virtual FGenericTeamId GetGenericTeamId() const override;
	
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

};
