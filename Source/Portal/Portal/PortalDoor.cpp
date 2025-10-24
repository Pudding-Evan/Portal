// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalDoor.h"

#include "MirrorAnimInstance.h"
#include "PortalCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "StateMachine/StateMachineComponent.h"

#include "Global/PGameplayTags.h"

APortalDoor::APortalDoor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SMDoor = CreateDefaultSubobject<UStaticMeshComponent>("Door");
	RootComponent = SMDoor;
	
	Plane = CreateDefaultSubobject<UStaticMeshComponent>("Plane");
	Plane->SetupAttachment(RootComponent);

	PortalCamera = CreateDefaultSubobject<USceneCaptureComponent2D>("PortalCamera");
	PortalCamera->SetupAttachment(RootComponent);

	ViewCamera = CreateDefaultSubobject<UCameraComponent>("PlayerCamera");
	ViewCamera->SetupAttachment(RootComponent);

	CrossingDetectionBox = CreateDefaultSubobject<UBoxComponent>("CrossingDetectionBox");
	CrossingDetectionBox->SetupAttachment(RootComponent);

	ActivateDetectionBox = CreateDefaultSubobject<UBoxComponent>("ActivateDetectionBox");
	ActivateDetectionBox->SetupAttachment(RootComponent);

	StateMachine = CreateDefaultSubobject<UStateMachineComponent>("StateMachineComponent");
}

void APortalDoor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Tags.Add(PortalTag);
}

void APortalDoor::BeginPlay()
{
	Super::BeginPlay();
	
	if (GEngine && GEngine->GameViewport)
	{
		FViewport* Viewport = GEngine->GameViewport->Viewport;
		Viewport->ViewportResizedEvent.AddUObject(this, &APortalDoor::OnViewportResized);
	}
	
	InitTextureTarget();
	
	ActivateDetectionBox->OnComponentBeginOverlap.AddDynamic(this, &APortalDoor::OnActivateBoxOverlapBegin);
	ActivateDetectionBox->OnComponentEndOverlap.AddDynamic(this, &APortalDoor::OnActivateBoxOverlapEnd);
	CrossingDetectionBox->OnComponentBeginOverlap.AddDynamic(this, &APortalDoor::OnCrossBoxOverlapBegin);
	CrossingDetectionBox->OnComponentEndOverlap.AddDynamic(this, &APortalDoor::OnCrossBoxOverlapEnd);
}

void APortalDoor::CreateMirrorCharacter()
{
	if (MirrorCharacterClass
		&& !MirrorCharacter)
	{
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.Owner = this;
		APortalCharacter* PCharacter = Cast<APortalCharacter>(UGameplayStatics::GetPlayerCharacter(this,0));
		MirrorCharacter = GetWorld()->SpawnActor<ACharacter>(MirrorCharacterClass,GetActorLocation(),GetActorRotation(),ActorSpawnParams);
		auto MirrorAnimInst = Cast<UMirrorAnimInstance>(MirrorCharacter->GetMesh()->GetAnimInstance());
		ensure(MirrorAnimInst);
		MirrorAnimInst->SourceMesh = PCharacter->GetMesh();
		MirrorCharacter->SetActorHiddenInGame(true);
	}
}

FTransform APortalDoor::CalculateMirroredRelativeTrans(const FTransform& InTransform, const FTransform& BaseTransform)
{
	FTransform LocalTrans = InTransform.GetRelativeTransform(BaseTransform);
	const FVector UpVector = BaseTransform.GetRotation().GetUpVector();
	FQuat RotUp180Quat(UpVector, FMath::DegreesToRadians(180.0f));
	FTransform  FMirroredLocalTrans;
	FMirroredLocalTrans.SetLocation(RotUp180Quat.RotateVector(LocalTrans.GetLocation()));
	FMirroredLocalTrans.SetRotation(RotUp180Quat * LocalTrans.GetRotation());

	return FMirroredLocalTrans;
}

void APortalDoor::UpdatePortalCameraTransform()
{
	APortalDoor* LinkDoor = GetLinkPortal();
	if (!LinkDoor)
	{
		return;
	}

	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this,0);
	FTransform CameraTransform = CameraManager->GetTransform();
	FTransform  FMirroredLocalTrans = CalculateMirroredRelativeTrans(CameraTransform,LinkDoor->GetActorTransform());
	PortalCamera->SetRelativeTransform(FMirroredLocalTrans);
}

void APortalDoor::UpdateMirrorCharacterTrans()
{
	APortalDoor* LinkDoor = GetLinkPortal();
	if (!LinkDoor)
	{
		return;
	}

	auto* Character = UGameplayStatics::GetPlayerCharacter(this,0);
	FTransform CharacterTransform = Character->GetActorTransform();
	FTransform  FMirroredLocalTrans = CalculateMirroredRelativeTrans(CharacterTransform,LinkDoor->GetActorTransform());
	MirrorCharacter->SetActorTransform(FMirroredLocalTrans * GetActorTransform());
}

void APortalDoor::UpdateViewCameraTransform()
{
	APortalDoor* LinkDoor = GetLinkPortal();
	if (!LinkDoor)
	{
		return;
	}
	
	APortalCharacter* PCharacter = Cast<APortalCharacter>(UGameplayStatics::GetPlayerCharacter(this,0));
	auto CharacterCam = PCharacter->GetFollowCamera();
	FTransform PlayerCameraTrans = CharacterCam->GetComponentTransform();
	FTransform  FMirroredLocalTrans = CalculateMirroredRelativeTrans(PlayerCameraTrans,LinkDoor->GetActorTransform());

	ViewCamera->SetWorldTransform(FMirroredLocalTrans * GetActorTransform());
}

void APortalDoor::InitTextureTarget()
{
	if (!RTPortal)
	{
		RTPortal = NewObject<UTextureRenderTarget2D>(this);
		UMaterialInstanceDynamic* DynamicMat = UMaterialInstanceDynamic::Create(MI_PortalPlane, this);
		DynamicMat->SetTextureParameterValue(FName("Texture"),RTPortal);
		Plane->SetMaterial(0, DynamicMat);
	}
	
	SetClipPlanes();
}

void APortalDoor::SetRenderTargetActive(const bool InActive)
{
	UMaterialInstanceDynamic* DynMat = Cast<UMaterialInstanceDynamic>(Plane->GetMaterial(0));
	if (!DynMat)
	{
		return;
	}
	APortalDoor* OtherLinkPortal = GetLinkPortal();
	if(!OtherLinkPortal)
	{
		return;
	}
	
	float ActiveValue = InActive ? 1.0f : 0.0f;
	DynMat->SetScalarParameterValue(TEXT("Active"), ActiveValue);
	OtherLinkPortal->PortalCamera->TextureTarget = InActive ? RTPortal : nullptr;
	if (GEngine && GEngine->GameViewport)
	{
		FVector2D ViewportSize;
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		int32 Width = ViewportSize.X;
		int32 Height = ViewportSize.Y;
		RTPortal->InitAutoFormat(Width, Height);
		RTPortal->ResizeTarget(Width, Height);
		RTPortal->UpdateResourceImmediate(true);
	}
}

APortalDoor* APortalDoor::GetLinkPortal() 
{
	if(LinkPortal.IsValid())
	{
		return LinkPortal.Get();
	}
	
	TArray<AActor*> OutActor{};
	UGameplayStatics::GetAllActorsOfClassWithTag(this,StaticClass(),LinkPortalTag,OutActor);
	if(OutActor.Num() > 0)
	{
		if(APortalDoor* FindDoor = Cast<APortalDoor>(OutActor[0]))
		{
			LinkPortal = FindDoor;
			return FindDoor;
		}
	}
	
	return nullptr;
}

USceneCaptureComponent2D* APortalDoor::GetLinkPortalCamera()
{
	auto Portal = GetLinkPortal();
	if (!Portal)
	{
		return nullptr;
	}
	
	return Portal->PortalCamera;
}

UCameraComponent* APortalDoor::GetLinkPlayerCamera()
{
	auto Portal = GetLinkPortal();
	if (!Portal)
	{
		return nullptr;
	}
	return Portal->ViewCamera;
}

void APortalDoor::SetClipPlanes() const
{
	PortalCamera->bEnableClipPlane = true;
	PortalCamera->ClipPlaneNormal = GetDoorForwardDirection();
	PortalCamera->ClipPlaneBase = GetActorLocation();
}

void APortalDoor::OnActivateBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (CheckIsLocalCharacter(Character))
	{
		StateMachine->TryChangeState(GameplayTags::Portal::Active);
		if (GetLinkPortal())
		{
			GetLinkPortal()->StateMachine->TryChangeState(GameplayTags::Portal::LinkActive);
		}
	}
}

void APortalDoor::OnActivateBoxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (CheckIsLocalCharacter(Character))
	{
		StateMachine->TryChangeState(GameplayTags::Portal::UnActive);
		if (GetLinkPortal())
		{
			GetLinkPortal()->StateMachine->TryChangeState(GameplayTags::Portal::UnActive);
		}
	}
}

void APortalDoor::OnCrossBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (CheckIsLocalCharacter(Character))
	{
		StateMachine->TryChangeState(GameplayTags::Portal::Crossing);
		if (GetLinkPortal())
		{
			GetLinkPortal()->StateMachine->TryChangeState(GameplayTags::Portal::LinkCrossing);
		}
	}
}

void APortalDoor::OnCrossBoxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (CheckIsLocalCharacter(Character))
	{
		FVector CharacterLocation = Character->GetActorLocation();
		float Dot = FVector::DotProduct(CharacterLocation - GetActorLocation(), GetDoorForwardDirection());
		if (Dot < 0)
		{
			StateMachine->TryChangeState(GameplayTags::Portal::PostCrossing);
			if (GetLinkPortal())
			{
				GetLinkPortal()->StateMachine->TryChangeState(GameplayTags::Portal::LinkPostCrossing);
			}
		}
		else
		{
			StateMachine->TryChangeState(GameplayTags::Portal::Active);
			if (GetLinkPortal())
			{
				GetLinkPortal()->StateMachine->TryChangeState(GameplayTags::Portal::LinkActive);
			}
		}
	}
}

void APortalDoor::TeleportCharacter(ACharacter* Character)
{
	FTransform CharacterTransform = Character->GetActorTransform();
	FTransform  FMirroredLocalTrans = CalculateMirroredRelativeTrans(CharacterTransform,GetActorTransform());
	if (!GetLinkPortal())
	{
		return;
	}

	float Speed = Character->GetVelocity().Length();
	FRotator OldVelocityDir = Character->GetVelocity().Rotation();
	const FVector UpVector = GetActorUpVector();
	
	// Teleport
	FTransform FinalTransform = FMirroredLocalTrans * GetLinkPortal()->GetTransform();
	auto PCharacter =  Cast<APortalCharacter>(Character);
	if (!PCharacter)
	{
		return;
	}

	// Player Controller
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this,0);
	FRotator  ControllerRot =  PlayerController->GetControlRotation();
	FVector OldCharacterDir =  PCharacter->GetActorForwardVector();
	FVector LocalControllerVec  =  ControllerRot.UnrotateVector(OldCharacterDir);

	FVector LocalVelocityVec = OldVelocityDir.UnrotateVector(OldCharacterDir);
	
	PCharacter->SetActorLocation(FinalTransform.GetLocation());
	PCharacter->SetActorRotation(FinalTransform.Rotator());

	FVector NewCharacterDir = FinalTransform.GetRotation().GetForwardVector();
	FVector FinalControlDir = (LocalControllerVec.Rotation()).RotateVector(NewCharacterDir);
	ControllerRot.Yaw = FinalControlDir.Rotation().Yaw;
	PlayerController->SetControlRotation(ControllerRot);
	
	// Velocity
	FVector FinalVelocityDir = (LocalVelocityVec.Rotation()).RotateVector(NewCharacterDir);
	Character->GetCharacterMovement()->Velocity = FinalVelocityDir * Speed;
	
}

void APortalDoor::DetachViewTarget(const bool bDetach)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this,0);
	if (bDetach)
	{
		// Cache this frame
		PlayerController->PlayerCameraManager->SetGameCameraCutThisFrame();
			
		PlayerController->SetViewTargetWithBlend(this,0);
	}else
	{

		PlayerController->PlayerCameraManager->SetGameCameraCutThisFrame();
		
		APortalCharacter* PCharacter = Cast<APortalCharacter>(UGameplayStatics::GetPlayerCharacter(this,0));
		PlayerController->SetViewTargetWithBlend(PCharacter,0);
	}
}

void APortalDoor::OnViewportResized(FViewport* Viewport, uint32 NewSize)
{
	if(!RTPortal)
	{
		return;
	}
	
	// 解析宽度和高度
	const uint32 Width = Viewport->GetSizeXY().X;
	const uint32 Height = Viewport->GetSizeXY().Y;
	if(Width != RTPortal->SizeX || Height != RTPortal->SizeY)
	{
		RTPortal->ResizeTarget(Width,Height);
	}
}

bool APortalDoor::CheckIsLocalCharacter(const ACharacter* Character) const
{
	if (!Character)
	{
		return false;
	}
	APortalCharacter* PCharacter = Cast<APortalCharacter>(UGameplayStatics::GetPlayerCharacter(this,0));
	return PCharacter == Character;
}
