// Fill out your copyright notice in the Description page of Project Settings.


#include "RBVVCharacter.h"

// Sets default values
ARBVVCharacter::ARBVVCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARBVVCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARBVVCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ARBVVCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ARBVVCharacter::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	if (EyeSocketName.IsNone()) Super::GetActorEyesViewPoint(OutLocation, OutRotation);
	if (!EyeSocketName.IsNone())
	{
		OutLocation = GetMesh()->GetSocketLocation(EyeSocketName);
		OutRotation = GetMesh()->GetSocketRotation(EyeSocketName);
	}
}
