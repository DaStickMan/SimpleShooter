// Fill out your copyright notice in the Description page of Project Settings.


#include "KillEmAllGameMode.h"
#include "EngineUtils.h"
#include "GameFramework/Controller.h"
#include "ShooterAIController.h"

void AKillEmAllGameMode::PawnKilled(APawn* PawnKilled) 
{
	Super::PawnKilled(PawnKilled);

	APlayerController* PlayerController = Cast<APlayerController>(PawnKilled->GetController());
	if (PlayerController != nullptr)
	{
		EndGame(false);
	}
	
	for (AShooterAIController* Controller : TActorRange<AShooterAIController>(GetWorld()))
	{
		if (!Controller->IsDead())
		{
			//ignores if there is at least one enemy alive
			return;
		}
	}

	//Killed every enemy
	EndGame(true);
}

void AKillEmAllGameMode::EndGame(bool bIsPlayerWinner)
{
	//gets a range of controllers (all controllers in the world)
	for (AController* Controller : TActorRange<AController>(GetWorld()))
	{
		Controller->GameHasEnded(Controller->GetPawn(), Controller->IsPlayerController() == bIsPlayerWinner);
	}
}
