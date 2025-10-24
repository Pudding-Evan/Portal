// Fill out your copyright notice in the Description page of Project Settings.


#include "StateMachineComponent.h"

#pragma optimize("", off)
// Sets default values for this component's properties
UStateMachineComponent::UStateMachineComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetTickGroup(TG_PostUpdateWork);
	// ...
}


// Called when the game starts
void UStateMachineComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!TransitionConfig)
	{
		return;
	}

	for (const auto& StateClass: TransitionConfig->AllStates)
	{
		// 实例化所有的State
		if (StateClass && StateClass->GetDefaultObject<UStateBase>())
		{
			FGameplayTag StateTag = StateClass->GetDefaultObject<UStateBase>()->GetStateTag();
			if (StateTag.IsValid() && !AllStates.Contains(StateTag))
			{
				if (UStateBase* NewStateInstance = NewObject<UStateBase>(this, StateClass))
				{
					NewStateInstance->Owner = GetOwner();
					AllStates.Add(StateTag, NewStateInstance);
				}
			}
		}
	}
	
	for (const auto& [InitialStateTag, TransitionStateTags] : TransitionConfig->TransitionRules)
	{
		// 2. 缓存状态切换规则
		TransitionRules.Add(InitialStateTag, TransitionStateTags);
	}
	
	// 3. 切换到初始状态
	if (TransitionConfig->InitialStateTag.IsValid())
	{
		TryChangeState(TransitionConfig->InitialStateTag);
	}
}


// Called every frame
void UStateMachineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentState && CurrentState->ShouldActive())
	{
		CurrentState->Update(DeltaTime);
	}
}

bool UStateMachineComponent::TryChangeState(const FGameplayTag NewStateTag)
{
	// 检查是否在AllStates Map中存在新状态的实例
	if (!AllStates.Contains(NewStateTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempted to change to a non-existent state: %s"), *NewStateTag.ToString());
		return false;
	}
	
	if (CurrentState)
	{
		// 查找当前状态的切换规则
		if (const auto& AllowedTransitions = TransitionRules.Find(CurrentState->GetStateTag()))
		{
			// 检查目标状态是否在允许的列表中
			bool bIsAllowed = AllowedTransitions->HasTag(NewStateTag);
			if (!bIsAllowed)
			{
				UE_LOG(LogTemp, Warning, TEXT("Transition from %s to %s is not allowed."), *CurrentState->GetStateTag().ToString(), *NewStateTag.ToString());
				return false;
			}
		}
	}
    
	UStateBase* NextState = AllStates[NewStateTag];
	if (NextState && NextState->ShouldActive())
	{
		// 退出当前状态
		FGameplayTag PreviousStateTag;
		if (CurrentState)
		{
			PreviousStateTag = CurrentState->GetStateTag();
			CurrentState->OnStateExited_Implementation(NewStateTag);
		}
        
		// 进入新状态
		CurrentState = NextState;
		CurrentState->OnStateEntered_Implementation(PreviousStateTag);
	}
	
	return true;
}
#pragma optimize("", on)
