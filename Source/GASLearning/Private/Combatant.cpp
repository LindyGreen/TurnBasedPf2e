// Fill out your copyright notice in the Description page of Project Settings.


#include "Combatant.h"
#include "TurnManagerComponent.h"
#include "Grid.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ACombatant::ACombatant()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACombatant::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACombatant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACombatant::SetupPlayerInputComponent(
	UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float ACombatant::RollInitiative()
{
	return Initiative + UKismetMathLibrary::RandomFloatInRange(1.0f, 20.0f);
}

