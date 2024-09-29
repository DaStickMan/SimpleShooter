// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "Gun.h"
#include "Components/CapsuleComponent.h"
#include "SimpleShooterGameModeBase.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (PrimaryGunClass == nullptr || SecondaryGunClass == nullptr)
	{
		return;
	}

	GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None);
	SetGun(1);

	Health = MaxHealth;		
}

bool AShooterCharacter::IsDead() const
{
	return Health <= 0;
}

float AShooterCharacter::GetHealthPercent() const
{
	return Health / MaxHealth;
}

float AShooterCharacter::GetAmmo() const
{
	return Ammo;
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Lookup"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Shoot"), EInputEvent::IE_Pressed, this, &AShooterCharacter::Shoot);
	PlayerInputComponent->BindAction<FFooDelegate>(TEXT("PrimaryGun"), EInputEvent::IE_Pressed, this, &AShooterCharacter::SetGun, 1);
	PlayerInputComponent->BindAction<FFooDelegate>(TEXT("SecondaryGun"), EInputEvent::IE_Pressed, this, &AShooterCharacter::SetGun, 2);
}

float AShooterCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageToApply = FMath::Min(Health, DamageToApply);
	Health -= DamageToApply;

	UE_LOG(LogTemp, Warning, TEXT("Damage taken! %f"), Health);

	if (IsDead())
	{
		ASimpleShooterGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ASimpleShooterGameModeBase>();

		if (GameMode != nullptr)
		{
			GameMode->PawnKilled(this);
		}

		DetachFromControllerPendingDestroy();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	return DamageToApply;
}

void AShooterCharacter::MoveForward(float AxisValue) 
{
	AddMovementInput(GetActorForwardVector() * AxisValue);	
}


void AShooterCharacter::MoveRight(float AxisValue)
{
	AddMovementInput(GetActorRightVector() * AxisValue);
}

void AShooterCharacter::Shoot()
{
	if (Ammo == 0)
		return;

	Ammo--;

	UE_LOG(LogTemp, Log, TEXT("Ammo: %i"), Ammo);
	
	if (PrimaryGun == nullptr && SecondaryGun)
	{
		SecondaryGun->PullTrigger();
	}
	else if(SecondaryGun == nullptr && PrimaryGun) 
	{
		PrimaryGun->PullTrigger();
	}
}

void AShooterCharacter::SetGun(int gunIndex)
{
	UE_LOG(LogTemp, Log, TEXT("Teste %i"), gunIndex);

	if (gunIndex == 1)
	{
		if (SecondaryGun)
		{
			SecondaryGun->Destroy();
			SecondaryGun = nullptr;
		}

		PrimaryGun = GetWorld()->SpawnActor<AGun>(PrimaryGunClass);
		PrimaryGun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
		PrimaryGun->SetOwner(this);
	}
	else
	{
		if (PrimaryGun)
		{
			PrimaryGun->Destroy();
			PrimaryGun = nullptr;
		}

		SecondaryGun = GetWorld()->SpawnActor<AGun>(SecondaryGunClass);
		SecondaryGun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
		SecondaryGun->SetOwner(this);
	}	
}
