// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "Tasks/IAblAbilityTask.h"

#include "ablSubSystem.generated.h"

USTRUCT()
struct FAblTaskScratchPadBucket
{
	GENERATED_BODY()
public:
	FAblTaskScratchPadBucket() : ScratchPadClass(nullptr), Instances() {};

	UPROPERTY()
	TSubclassOf<UAblAbilityTaskScratchPad> ScratchPadClass;

	UPROPERTY()
	TArray<UAblAbilityTaskScratchPad*> Instances;
};

USTRUCT()
struct FAblAbilityScratchPadBucket
{
	GENERATED_BODY()
public:
	FAblAbilityScratchPadBucket() : ScratchPadClass(nullptr), Instances() {};

	UPROPERTY()
	TSubclassOf<UAblAbilityScratchPad> ScratchPadClass;

	UPROPERTY()
	TArray<UAblAbilityScratchPad*> Instances;
};


UCLASS()
class UAblScratchPadSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	UAblScratchPadSubsystem(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual ~UAblScratchPadSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Able")
	UAblAbilityTaskScratchPad* FindOrConstructTaskScratchPad(TSubclassOf<UAblAbilityTaskScratchPad>& Class);

	UAblAbilityScratchPad* FindOrConstructAbilityScratchPad(TSubclassOf<UAblAbilityScratchPad>& Class);

	void ReturnTaskScratchPad(UAblAbilityTaskScratchPad* Scratchpad);

	void ReturnAbilityScratchPad(UAblAbilityScratchPad* Scratchpad);

private:
	FAblTaskScratchPadBucket* GetTaskBucketByClass(TSubclassOf<UAblAbilityTaskScratchPad>& Class);
	FAblAbilityScratchPadBucket* GetAbilityBucketByClass(TSubclassOf<UAblAbilityScratchPad>& Class);
	uint32 GetTotalScratchPads() const;

	UPROPERTY(Transient)
	TArray<FAblTaskScratchPadBucket> m_TaskBuckets;

	UPROPERTY(Transient)
	TArray<FAblAbilityScratchPadBucket> m_AbilityBuckets;

	UPROPERTY(Transient)
	const UAbleSettings* m_Settings;
};

UCLASS()
class UAblAbilityContextSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	UAblAbilityContextSubsystem(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual ~UAblAbilityContextSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Able")
	UAblAbilityContext* FindOrConstructContext();

	void ReturnContext(UAblAbilityContext* Context);
private:
	UPROPERTY(Transient)
	TArray<UAblAbilityContext*> m_AvailableContexts;

	UPROPERTY(Transient)
	const UAbleSettings* m_Settings;
};
