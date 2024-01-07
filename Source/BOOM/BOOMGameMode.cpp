// Copyright Epic Games, Inc. All Rights Reserved.

#include "BOOMGameMode.h"
#include "Character/BOOMCharacter.h"
#include "UObject/ConstructorHelpers.h"

ABOOMGameMode::ABOOMGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
