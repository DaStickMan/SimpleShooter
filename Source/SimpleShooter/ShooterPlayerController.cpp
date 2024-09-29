// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterPlayerController.h"
#include "Blueprint/UserWidget.h"

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UUserWidget* HUDScreen = CreateWidget(this, HUDScreenClass);

	if (HUDScreen != nullptr)
	{
		HUDScreen->AddToViewport();
	}
}

void AShooterPlayerController::GameHasEnded(AActor* EndGameFocus, bool bIsWinner)
{
	Super::GameHasEnded(EndGameFocus, bIsWinner);

	UUserWidget* UserScreen;

	if (bIsWinner)
	{
		UserScreen = CreateWidget(this, WinScreenClass);
	}
	else 
	{
		UserScreen = CreateWidget(this, LoseScreenClass);
	}

	if (UserScreen != nullptr)
	{
		UserScreen->AddToViewport();
	}

	GetWorldTimerManager().SetTimer(RestartTimer, this, &APlayerController::RestartLevel, RestartDelay);
}
