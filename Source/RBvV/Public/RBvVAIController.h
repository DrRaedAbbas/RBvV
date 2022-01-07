// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GenericTeamAgentInterface.h"

#include "RBvVAIController.generated.h"

/**
 * 
 */
UCLASS()
class RBVV_API ARBvVAIController : public AAIController
{
	GENERATED_BODY()
	ARBvVAIController();

protected:

	//FGenericTeamId NewTID;
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	
public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "RBVV")
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;

	UFUNCTION(BlueprintCallable, Category = "RBVV")
	virtual FGenericTeamId GetGenericTeamId() const override;

	UPROPERTY(BlueprintReadOnly, Category = "RBVV")
	FGenericTeamId NewTID;
	
	UPROPERTY(BlueprintReadWrite, Category = "RBVV")
	class ARBVVCharacter* Agent;
	
	/*UFUNCTION(BlueprintCallable, Category = "RBVV")
	void SetTeamID(uint8 NewTeamID);

	UFUNCTION(BlueprintCallable, Category = "RBVV")
	void GetTeamID();*/
};
