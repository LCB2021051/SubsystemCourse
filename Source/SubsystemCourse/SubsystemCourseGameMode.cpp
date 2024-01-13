// Copyright Epic Games, Inc. All Rights Reserved.

#include "SubsystemCourseGameMode.h"
#include "SubsystemCourseCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASubsystemCourseGameMode::ASubsystemCourseGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
