// Copyright Epic Games, Inc. All Rights Reserved.

#include "Monster_ShooterGameMode.h"
#include "Monster_ShooterCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMonster_ShooterGameMode::AMonster_ShooterGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
