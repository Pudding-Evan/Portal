#include "PortalState.h"

#include "MirrorAnimInstance.h"
#include "PortalCharacter.h"
#include "PortalDoor.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Global/GameTraceChannel.h"
#include "StateMachine/StateMachineComponent.h"
#include "Kismet/GameplayStatics.h"

/*
 * PortalUnActiveState
 */

UPortalUnActiveState::UPortalUnActiveState()
{
	StateTag = GameplayTags::Portal::UnActive;
}

void UPortalUnActiveState::OnStateEntered_Implementation(const FGameplayTag& FromState)
{
	Super::OnStateEntered_Implementation(FromState);
	APortalDoor* PortalDoor = static_cast<APortalDoor*>(Owner.Get());
	ensure(PortalDoor);
	PortalDoor->SetRenderTargetActive(false);
}

void UPortalUnActiveState::OnStateExited_Implementation(const FGameplayTag& ToState)
{
	Super::OnStateExited_Implementation(ToState);
	APortalDoor* PortalDoor = static_cast<APortalDoor*>(Owner.Get());
	PortalDoor->SetRenderTargetActive(true);
}

/*
 * PortalActiveState
 */

UPortalActiveState::UPortalActiveState()
{
	StateTag =GameplayTags::Portal::Active;
}

void UPortalActiveState::OnStateEntered_Implementation(const FGameplayTag& FromState)
{
	Super::OnStateEntered_Implementation(FromState);

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this,0);
	APortalCharacter* PCharacter = Cast<APortalCharacter>(UGameplayStatics::GetPlayerCharacter(this,0));
	PlayerController->SetViewTargetWithBlend(PCharacter,0);
}

void UPortalActiveState::Update(float DeltaTime)
{
	Super::Update(DeltaTime);
	if (APortalDoor* PortalDoor = static_cast<APortalDoor*>(Owner.Get()))
	{
		PortalDoor->UpdatePortalCameraTransform();
	}
}

/*
 * PortalLinkActiveState
 */

UPortalLinkActiveState::UPortalLinkActiveState()
{
	StateTag = GameplayTags::Portal::LinkActive;
}

void UPortalLinkActiveState::OnStateEntered_Implementation(const FGameplayTag& FromState)
{
	Super::OnStateEntered_Implementation(FromState);
	if (APortalDoor* PortalDoor = static_cast<APortalDoor*>(Owner.Get()))
	{
		PortalDoor->CreateMirrorCharacter();
	}
}

void UPortalLinkActiveState::Update(float DeltaTime)
{
	Super::Update(DeltaTime);
	if (APortalDoor* PortalDoor = static_cast<APortalDoor*>(Owner.Get()))
	{
		PortalDoor->UpdatePortalCameraTransform();
	}
}

/*
 * PortalCrossingState
 */

UPortalCrossingState::UPortalCrossingState()
{
	StateTag = GameplayTags::Portal::Crossing;
}

void UPortalCrossingState::OnStateExited_Implementation(const FGameplayTag& ToState)
{
	Super::OnStateExited_Implementation(ToState);

	if (ToState == GameplayTags::Portal::PostCrossing)
	{
		// Detach view target 
		APortalDoor* PortalDoor = static_cast<APortalDoor*>(Owner.Get());
		APortalCharacter* PCharacter = Cast<APortalCharacter>(UGameplayStatics::GetPlayerCharacter(this,0));
		
		PortalDoor->TeleportCharacter(PCharacter);
		PortalDoor->DetachViewTarget(true);
	}
}

void UPortalCrossingState::Update(float DeltaTime)
{
	Super::Update(DeltaTime);
	if (APortalDoor* PortalDoor = static_cast<APortalDoor*>(Owner.Get()))
	{
		PortalDoor->UpdatePortalCameraTransform();
	}
}

/*
 * PortalLinkCrossingState
 */

UPortalLinkCrossingState::UPortalLinkCrossingState()
{
	StateTag = GameplayTags::Portal::LinkCrossing;
}

void UPortalLinkCrossingState::OnStateEntered_Implementation(const FGameplayTag& FromState)
{
	Super::OnStateEntered_Implementation(FromState);
	APortalDoor* PortalDoor = static_cast<APortalDoor*>(Owner.Get());
	ensure(PortalDoor);
	PortalDoor->MirrorCharacter->GetMesh()->GetAnimInstance()->UpdateAnimation(0,true);
	PortalDoor->MirrorCharacter->GetMesh()->RefreshBoneTransforms();
	PortalDoor->MirrorCharacter->SetActorHiddenInGame(false);
}

void UPortalLinkCrossingState::OnStateExited_Implementation(const FGameplayTag& ToState)
{
	Super::OnStateExited_Implementation(ToState);
	APortalDoor* PortalDoor = static_cast<APortalDoor*>(Owner.Get());
	ensure(PortalDoor);
	PortalDoor->MirrorCharacter->SetActorHiddenInGame(true);
}

void UPortalLinkCrossingState::Update(float DeltaTime)
{
	Super::Update(DeltaTime);
	if (APortalDoor* PortalDoor = static_cast<APortalDoor*>(Owner.Get()))
	{
		PortalDoor->UpdatePortalCameraTransform();
		PortalDoor->UpdateMirrorCharacterTrans();
	}
}

/*
 * PortalPostCrossingState
 */
UPortalPostCrossingState::UPortalPostCrossingState()
{
	StateTag = GameplayTags::Portal::PostCrossing;
}

void UPortalPostCrossingState::OnStateEntered_Implementation(const FGameplayTag& FromState)
{
	Super::OnStateEntered_Implementation(FromState);
	
	APortalDoor* PortalDoor = static_cast<APortalDoor*>(Owner.Get());
	PortalDoor->UpdateViewCameraTransform();
}

void UPortalPostCrossingState::OnStateExited_Implementation(const FGameplayTag& ToState)
{
	Super::OnStateExited_Implementation(ToState);

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this,0);
	APortalCharacter* PCharacter = Cast<APortalCharacter>(UGameplayStatics::GetPlayerCharacter(this,0));
	PlayerController->SetViewTargetWithBlend(PCharacter,0);
}

void UPortalPostCrossingState::Update(float DeltaTime)
{
	Super::Update(DeltaTime);
	
	// Update Player Camera
	APortalDoor* PortalDoor = static_cast<APortalDoor*>(Owner.Get());
	if (!PortalDoor)
	{
		return;
	}
	
	PortalDoor->UpdatePortalCameraTransform();
	PortalDoor->UpdateViewCameraTransform();
}

/*
 * PortalLinkPostCrossingState
 */

UPortalLinkPostCrossingState::UPortalLinkPostCrossingState()
{
	StateTag = GameplayTags::Portal::LinkPostCrossing;
}

void UPortalLinkPostCrossingState::OnStateEntered_Implementation(const FGameplayTag& FromState)
{
	Super::OnStateEntered_Implementation(FromState);

	APortalCharacter* PCharacter = Cast<APortalCharacter>(UGameplayStatics::GetPlayerCharacter(this,0));
	ensure(PCharacter);
	if (USpringArmComponent* SpringArm = PCharacter->GetCameraBoom())
	{
		SpringArm->bDoCollisionTest = false;
	}
	
}

void UPortalLinkPostCrossingState::OnStateExited_Implementation(const FGameplayTag& ToState)
{
	Super::OnStateExited_Implementation(ToState);
	APortalCharacter* PCharacter = Cast<APortalCharacter>(UGameplayStatics::GetPlayerCharacter(this,0));
	ensure(PCharacter);
	if (USpringArmComponent* SpringArm = PCharacter->GetCameraBoom())
	{
		SpringArm->bDoCollisionTest = true;
	}
}

void UPortalLinkPostCrossingState::Update(float DeltaTime)
{
	Super::Update(DeltaTime);
	APortalDoor* PortalDoor = static_cast<APortalDoor*>(Owner.Get());
	PortalDoor->UpdatePortalCameraTransform();

	// Change State
	APortalCharacter* PCharacter = Cast<APortalCharacter>(UGameplayStatics::GetPlayerCharacter(this,0));
	if (!PCharacter)
	{
		return;
	}
	
	FVector StartTraceLoc = PCharacter->GetFollowCamera()->GetComponentLocation();
	FVector EndTraceLoc = PCharacter->GetActorLocation();
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartTraceLoc, EndTraceLoc,PORTAL_TRACE,QueryParams);
	if (!bHit)
	{
		PortalDoor->StateMachine->TryChangeState(GameplayTags::Portal::Active);
		if (auto LinkDoor = PortalDoor->GetLinkPortal())
		{
			LinkDoor->StateMachine->TryChangeState(GameplayTags::Portal::LinkActive);
		}
	}
}




