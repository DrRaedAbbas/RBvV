// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "RBVVCharacter.generated.h"

UCLASS()
class RBVV_API ARBVVCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARBVVCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void FellOutOfWorld(const UDamageType& dmgType) override;
	UFUNCTION(BlueprintImplementableEvent)
	void OnFallingFromHigh();
	
	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="RBVV")
	FName EyeSocketName;
};
