// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PortalGameMode.generated.h"

/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract)
class APortalGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	/** Constructor */
	APortalGameMode();
};



