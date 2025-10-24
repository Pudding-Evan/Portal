// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PortalDoor.generated.h"

class APortalCharacter;
class UStateMachineComponent;
class UCameraComponent;
class UBoxComponent;
class UMaterialInterface;

UCLASS()
class PORTAL_API APortalDoor : public AActor
{
	GENERATED_BODY()

public:
	APortalDoor();

protected:

	virtual void PostInitializeComponents() override;
	
	virtual void BeginPlay() override;

	static FTransform CalculateMirroredRelativeTrans(const FTransform& InTransform, const FTransform& BaseTransform);

public:

	void InitTextureTarget();
	void SetRenderTargetActive(bool InActive);
	
	void UpdatePortalCameraTransform();
	void UpdateMirrorCharacterTrans();
	void UpdateViewCameraTransform();
	
	UFUNCTION(Blueprintable)
	APortalDoor* GetLinkPortal();

	UFUNCTION(BlueprintCallable)
	FVector GetDoorForwardDirection() const {return GetActorForwardVector();}
	
	UFUNCTION(BlueprintCallable)
	void OnActivateBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void OnActivateBoxOverlapEnd(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void OnCrossBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void OnCrossBoxOverlapEnd(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION(BlueprintCallable)
	void TeleportCharacter(ACharacter* Character);

	void CreateMirrorCharacter();
	
	void DetachViewTarget(bool bDetach);
	
	UFUNCTION(BlueprintCallable)
	void SetClipPlanes() const;
	
	UPROPERTY(EditAnywhere,Category = "Portal | Config")
	FName PortalTag{};

	UPROPERTY(EditAnywhere,Category = "Portal | Config")
	FName LinkPortalTag{};

	UPROPERTY(EditAnywhere,Category = "Portal | Config")
	UMaterialInterface* MI_PortalPlane;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	UStaticMeshComponent* SMDoor{nullptr};

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	UStaticMeshComponent* Plane{nullptr}; // Plane Add Mat

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	UBoxComponent* CrossingDetectionBox;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	UBoxComponent* ActivateDetectionBox;
	
	UFUNCTION(BlueprintCallable)
	USceneCaptureComponent2D* GetLinkPortalCamera();

	UFUNCTION(BlueprintCallable)
	UCameraComponent* GetLinkPlayerCamera();

protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	TSubclassOf<ACharacter> MirrorCharacterClass;
public:
	UPROPERTY()
	TWeakObjectPtr<APortalDoor> LinkPortal{nullptr};
	
	UPROPERTY(EditAnywhere,Transient)
	USceneCaptureComponent2D* PortalCamera{nullptr};

	UPROPERTY(EditAnywhere,Transient)
	UCameraComponent* ViewCamera{nullptr};

	UPROPERTY(BlueprintReadOnly,Transient)
	UTextureRenderTarget2D* RTPortal{nullptr};

	UPROPERTY(BlueprintReadOnly,Transient)
	ACharacter* MirrorCharacter{nullptr};

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	UStateMachineComponent* StateMachine{nullptr};

	UPROPERTY()
	bool bDetachCamera{false};
	
	void OnViewportResized(FViewport* Viewport, uint32 NewSize);
	
	bool CheckIsLocalCharacter(const ACharacter* Character) const;
	
	TArray<TWeakObjectPtr<ACharacter>> PrepareTeleportCharacter;
};
