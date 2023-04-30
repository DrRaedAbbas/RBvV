// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#include "ablSubSystem.h"
#include "ablAbilityContext.h"
#include "ablSettings.h"

UAblScratchPadSubsystem::UAblScratchPadSubsystem(const FObjectInitializer& ObjectInitializer)
: m_Settings(nullptr)
{

}

UAblScratchPadSubsystem::~UAblScratchPadSubsystem()
{

}

void UAblScratchPadSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	m_Settings = GetDefault<UAbleSettings>();
}

UAblAbilityTaskScratchPad* UAblScratchPadSubsystem::FindOrConstructTaskScratchPad(TSubclassOf<UAblAbilityTaskScratchPad>& Class)
{
	if (m_Settings && !m_Settings->GetAllowScratchPadReuse())
	{
		return NewObject<UAblAbilityTaskScratchPad>(this, *Class);
	}

	UAblAbilityTaskScratchPad* OutInstance = nullptr;
	if (FAblTaskScratchPadBucket* ExistingBucket = GetTaskBucketByClass(Class))
	{
		if (ExistingBucket->Instances.Num())
		{
			OutInstance = ExistingBucket->Instances.Pop(false);
		}
		else
		{
			// Ran out of Instances, make one.
			OutInstance = NewObject<UAblAbilityTaskScratchPad>(this, *Class);
		}
	}
	else
	{
		FAblTaskScratchPadBucket& NewBucket = m_TaskBuckets.AddDefaulted_GetRef();
		NewBucket.ScratchPadClass = Class;
		OutInstance = NewObject<UAblAbilityTaskScratchPad>(this, *Class);
	}

	check(OutInstance);
	return OutInstance;
}

UAblAbilityScratchPad* UAblScratchPadSubsystem::FindOrConstructAbilityScratchPad(TSubclassOf<UAblAbilityScratchPad>& Class)
{
	if (m_Settings && !m_Settings->GetAllowScratchPadReuse())
	{
		return NewObject<UAblAbilityScratchPad>(this, *Class);
	}

	UAblAbilityScratchPad* OutInstance = nullptr;
	if (FAblAbilityScratchPadBucket* ExistingBucket = GetAbilityBucketByClass(Class))
	{
		if (ExistingBucket->Instances.Num())
		{
			OutInstance = ExistingBucket->Instances.Pop(false);
		}
		else
		{
			// Ran out of Instances, make one.
			OutInstance = NewObject<UAblAbilityScratchPad>(this, *Class);
		}
	}
	else
	{
		FAblAbilityScratchPadBucket& NewBucket = m_AbilityBuckets.AddDefaulted_GetRef();
		NewBucket.ScratchPadClass = Class;
		OutInstance = NewObject<UAblAbilityScratchPad>(this, *Class);
	}

	check(OutInstance);
	return OutInstance;
}

void UAblScratchPadSubsystem::ReturnTaskScratchPad(UAblAbilityTaskScratchPad* Scratchpad)
{
	if (m_Settings)
	{
		if (!m_Settings->GetAllowScratchPadReuse())
		{
			return;
		}

		if (m_Settings->GetMaxScratchPadPoolSize() > 0U && GetTotalScratchPads() > m_Settings->GetMaxScratchPadPoolSize())
		{
			return;
		}
	}

	TSubclassOf<UAblAbilityTaskScratchPad> ClassSubClass = Scratchpad->GetClass();
	if (FAblTaskScratchPadBucket* Bucket = GetTaskBucketByClass(ClassSubClass))
	{
		Bucket->Instances.Push(Scratchpad);
	}

	// If we don't have a bucket then we somehow mixed worlds... which doesn't make sense. Just let it release through the GC system.
}

void UAblScratchPadSubsystem::ReturnAbilityScratchPad(UAblAbilityScratchPad* Scratchpad)
{
	if (m_Settings)
	{
		if (!m_Settings->GetAllowScratchPadReuse())
		{
			return;
		}

		if (m_Settings->GetMaxScratchPadPoolSize() > 0U && GetTotalScratchPads() > m_Settings->GetMaxScratchPadPoolSize())
		{
			return;
		}
	}

	TSubclassOf<UAblAbilityScratchPad> ClassSubClass = Scratchpad->GetClass();
	if (FAblAbilityScratchPadBucket* Bucket = GetAbilityBucketByClass(ClassSubClass))
	{
		Bucket->Instances.Push(Scratchpad);
	}

	// If we don't have a bucket then we somehow mixed worlds... which doesn't make sense. Just let it release through the GC system.
}

FAblTaskScratchPadBucket* UAblScratchPadSubsystem::GetTaskBucketByClass(TSubclassOf<UAblAbilityTaskScratchPad>& Class)
{
	if (!Class.Get())
	{
		return nullptr;
	}

	for (FAblTaskScratchPadBucket& Bucket : m_TaskBuckets)
	{
		if (Class->GetFName() == Bucket.ScratchPadClass->GetFName())
		{
			return &Bucket;
		}
	}

	return nullptr;
}

FAblAbilityScratchPadBucket* UAblScratchPadSubsystem::GetAbilityBucketByClass(TSubclassOf<UAblAbilityScratchPad>& Class)
{
	if (!Class.Get())
	{
		return nullptr;
	}

	for (FAblAbilityScratchPadBucket& Bucket : m_AbilityBuckets)
	{
		if (Class->GetFName() == Bucket.ScratchPadClass->GetFName())
		{
			return &Bucket;
		}
	}

	return nullptr;
}

uint32 UAblScratchPadSubsystem::GetTotalScratchPads() const
{
	uint32 TotalAmount = 0;

	for (const FAblAbilityScratchPadBucket& Bucket : m_AbilityBuckets)
	{
		TotalAmount += Bucket.Instances.Num();
	}

	for (const FAblTaskScratchPadBucket& Bucket : m_TaskBuckets)
	{
		TotalAmount += Bucket.Instances.Num();
	}

	return TotalAmount;
}

UAblAbilityContextSubsystem::UAblAbilityContextSubsystem(const FObjectInitializer& ObjectInitializer)
: m_Settings(nullptr)
{

}

UAblAbilityContextSubsystem::~UAblAbilityContextSubsystem()
{

}

void UAblAbilityContextSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	m_Settings = GetDefault<UAbleSettings>();
	if (m_Settings && m_Settings->GetInitialContextPoolSize() > 0)
	{
		m_AvailableContexts.Reserve(m_Settings->GetInitialContextPoolSize());
		for (uint32 i = 0; i < m_Settings->GetInitialContextPoolSize(); ++i)
		{
			UAblAbilityContext* Context = NewObject<UAblAbilityContext>(this);
			m_AvailableContexts.Push(Context);
		}
	}
}

UAblAbilityContext* UAblAbilityContextSubsystem::FindOrConstructContext()
{
	UAblAbilityContext* Context = nullptr;
	if (m_Settings && !m_Settings->GetAllowAbilityContextReuse())
	{
		Context = NewObject<UAblAbilityContext>(this);
	}
	else
	{
		if (m_AvailableContexts.Num() != 0)
		{
			Context = m_AvailableContexts.Pop(false);
		}
		else
		{
			Context = NewObject<UAblAbilityContext>(this);
		}
	}

	return Context;
}

void UAblAbilityContextSubsystem::ReturnContext(UAblAbilityContext* Context)
{
	if (m_Settings && !m_Settings->GetAllowAbilityContextReuse())
	{
		if (!m_Settings->GetAllowAbilityContextReuse())
		{
			return;
		}

		if (m_Settings->GetMaxContextPoolSize() > 0U && (uint32)m_AvailableContexts.Num() >= m_Settings->GetMaxContextPoolSize())
		{
			return;
		}
	}

	m_AvailableContexts.Push(Context);
}
