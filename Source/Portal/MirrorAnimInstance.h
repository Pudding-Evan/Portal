// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MirrorAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PORTAL_API UMirrorAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable)
	USkeletalMeshComponent* GetSourceMesh() const {return SourceMesh.Get();}

	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<USkeletalMeshComponent> SourceMesh;
};
