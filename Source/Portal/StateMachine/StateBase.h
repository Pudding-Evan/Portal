// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "StateBase.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class PORTAL_API UStateBase : public UObject
{
	GENERATED_BODY()
	
public:
	
	/** State Name。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Machine")
	FGameplayTag StateTag;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnStateEntered(const FGameplayTag& FromState);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnStateExited(const FGameplayTag& ToState);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool ShouldActive();

	virtual void Update(float DeltaTime);
	
	virtual bool CanUpdate() const;
	
	UFUNCTION(BlueprintPure, Category = "State Machine")
	FGameplayTag GetStateTag() const { return StateTag; }
	
	UPROPERTY(Transient)
	TObjectPtr<UObject> Owner;

private:
	bool bActive {false};
};

USTRUCT(BlueprintType)
struct FStateTransition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State Machine")
	FGameplayTag InitialStateTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State Machine")
	FGameplayTagContainer TransitionStateTags;
	
};

UCLASS(BlueprintType)
class PORTAL_API UStateFlowDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State Machine")
	FGameplayTag InitialStateTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State Machine")
	TArray<FStateTransition> TransitionRules;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Machine")
	TArray<TSubclassOf<UStateBase>> AllStates;
};