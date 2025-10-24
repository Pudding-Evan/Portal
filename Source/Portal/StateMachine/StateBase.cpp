// Fill out your copyright notice in the Description page of Project Settings.


#include "StateBase.h"

void UStateBase::OnStateEntered_Implementation(const FGameplayTag& FromState)
{
	bActive = true;
	UE_LOG(LogTemp,Display,TEXT("[OwnerName: %s] : UStateBase::OnStateEntered_Implementation() FromState %s, ToState %s"),
		*Owner->GetName(), *FromState.GetTagName().ToString(),*StateTag.GetTagName().ToString());
}

void UStateBase::OnStateExited_Implementation(const FGameplayTag& ToState)
{
	bActive = false;
	UE_LOG(LogTemp,Display,TEXT("[OwnerName: %s] : UStateBase::OnStateExited_Implementation() FromState %s,ToState: %s"),
		*Owner->GetName(),*StateTag.GetTagName().ToString(),*ToState.GetTagName().ToString());
	
}

bool UStateBase::ShouldActive_Implementation()
{
	return true;
}

void UStateBase::Update(float DeltaTime)
{
	
}

bool UStateBase::CanUpdate() const
{
	return bActive;
}