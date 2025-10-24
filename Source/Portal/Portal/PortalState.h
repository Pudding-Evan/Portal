#pragma once
#include "CoreMinimal.h"
#include "StateMachine/StateBase.h"
#include "Global/PGameplayTags.h"
#include "PortalState.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PORTAL_API UPortalUnActiveState : public UStateBase
{
	GENERATED_BODY()
public:

	UPortalUnActiveState();
	
	virtual void OnStateEntered_Implementation(const FGameplayTag& FromState) override;

	virtual void OnStateExited_Implementation(const FGameplayTag& ToState) override;

	virtual bool CanUpdate() const override{return false;};
};


UCLASS(Blueprintable, BlueprintType)
class PORTAL_API UPortalActiveState : public UStateBase
{
	GENERATED_BODY()
public:

	UPortalActiveState();
	
	virtual void OnStateEntered_Implementation(const FGameplayTag& FromState) override;
	
	virtual void Update(float DeltaTime) override;
};

UCLASS(Blueprintable, BlueprintType)
class PORTAL_API UPortalLinkActiveState : public UStateBase
{
	GENERATED_BODY()
public:
	
	UPortalLinkActiveState();
	
	virtual void OnStateEntered_Implementation(const FGameplayTag& FromState) override;
	
	virtual void Update(float DeltaTime) override;
};

UCLASS(Blueprintable, BlueprintType)
class PORTAL_API UPortalCrossingState : public UStateBase
{
	GENERATED_BODY()
public:

	UPortalCrossingState();
	
	virtual void OnStateExited_Implementation(const FGameplayTag& ToState) override;
	
	virtual void Update(float DeltaTime) override;
};

UCLASS(Blueprintable, BlueprintType)
class PORTAL_API UPortalLinkCrossingState : public UStateBase
{
	GENERATED_BODY()
public:
	
	UPortalLinkCrossingState();

	virtual void OnStateEntered_Implementation(const FGameplayTag& FromState) override;

	virtual void OnStateExited_Implementation(const FGameplayTag& ToState) override;

	virtual void Update(float DeltaTime) override;
};

UCLASS(Blueprintable, BlueprintType)
class PORTAL_API UPortalPostCrossingState : public UStateBase
{
	GENERATED_BODY()
public:
	UPortalPostCrossingState();

	virtual void OnStateEntered_Implementation(const FGameplayTag& FromState) override;

	virtual void OnStateExited_Implementation(const FGameplayTag& ToState) override;

	virtual void Update(float DeltaTime) override;
};

UCLASS(Blueprintable, BlueprintType)
class PORTAL_API UPortalLinkPostCrossingState : public UStateBase
{
	GENERATED_BODY()
public:
	UPortalLinkPostCrossingState();

	virtual void OnStateEntered_Implementation(const FGameplayTag& FromState) override;

	virtual void OnStateExited_Implementation(const FGameplayTag& ToState) override;
	
	virtual void Update(float DeltaTime) override;
};