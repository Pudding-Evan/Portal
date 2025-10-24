// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StateBase.h"
#include "Components/ActorComponent.h"
#include "StateMachineComponent.generated.h"


class UStateBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PORTAL_API UStateMachineComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStateMachineComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintPure, Category = "State Machine")
	bool TryChangeState(FGameplayTag NewStateTag);

	UFUNCTION(BlueprintPure, Category = "State Machine")
	UStateBase* GetCurrentState(){return CurrentState;}
	
protected:
	
	UPROPERTY(Transient)
	TObjectPtr<UStateBase> CurrentState;

	/** 预先创建的所有状态实例。 */
	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<UStateBase>> AllStates;
	
	UPROPERTY(EditDefaultsOnly, Category = "State Machine|State")
	UStateFlowDataAsset* TransitionConfig;

private:
	
	TMap<FGameplayTag, FGameplayTagContainer> TransitionRules;
};
