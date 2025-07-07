// Fill out your copyright notice in the Description page of Project Settings.

#include "StructsAndEnums/FL_Spells_CPP.h"
#include "Grid.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "StructsAndEnums/F_TileData.h"

FS_IntPointArray UFL_Spells_CPP::LineTraceSpells(const FS_IntPointArray& InputIntpointArray, AGrid* Grid, FIntPoint Origin)
{
	FS_IntPointArray TempArray;
	
	if (!Grid)
	{
		return TempArray;
	}
	
	// Get origin tile data
	const FS_TileData* OriginTileData = Grid->GridTiles.Find(Origin);
	if (!OriginTileData)
	{
	UE_LOG(LogTemp, Error, TEXT ("Somehow player chose origin tile that is invalid in FL_Spells CPP"));
		return TempArray;
	}
	
	// Extract origin location from transform
	FVector OriginLocation = OriginTileData->Transform.GetLocation();
	
	// Single starting point with Z offset
	FVector StartPosition = OriginLocation + FVector(0.0, 0.0, 100.0);
	
	// Loop through each input point
	for (const FIntPoint& TargetPoint : InputIntpointArray.IntPointArray)
	{
		// Check if tile exists in GridTiles map
		const FS_TileData* TargetTileData = Grid->GridTiles.Find(TargetPoint);
		if (!TargetTileData)
		{
			continue; // Skip to next tile if not found
		}
		
		// Extract target location and scale from transform
		FVector TargetLocation, TargetScale;
		FRotator TargetRotation;
		UKismetMathLibrary::BreakTransform(TargetTileData->Transform, TargetLocation, TargetRotation, TargetScale);
		
		// Base target position with Z offset
		FVector BaseTargetPosition = TargetLocation + FVector(0.0, 0.0, 100.0);
		
		// Define 5 endpoint offsets using target tile's scale
		TArray<FVector2D> EndpointOffsets;
		EndpointOffsets.Add(FVector2D(0.0, 0.0)); // Center
		EndpointOffsets.Add(FVector2D(-50.0 * TargetScale.X, 0.0)); // Left
		EndpointOffsets.Add(FVector2D(0.0, 50.0 * TargetScale.Y)); // Up
		EndpointOffsets.Add(FVector2D(0.0, -50.0 * TargetScale.Y)); // Down
		EndpointOffsets.Add(FVector2D(50.0 * TargetScale.X, 0.0)); // Right
		
		// Test line traces to each of the 5 endpoints
		bool bHasLineOfSight = false;
		for (const FVector2D& Offset : EndpointOffsets)
		{
			// Calculate endpoint with offset
			FVector EndPosition = BaseTargetPosition + FVector(Offset.X, Offset.Y, 0.0);
			
			// Perform line trace from single start to this endpoint
			FHitResult HitResult;
			UKismetSystemLibrary::LineTraceSingle(
				Grid->GetWorld(),
				StartPosition,
				EndPosition,
				UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel2), // TraceTypeQuery2 is GRID as per DefaultEngine.ini
				false, // bTraceComplex
				TArray<AActor*>(), // ActorsToIgnore
				EDrawDebugTrace::None, // DrawDebugType
				HitResult,
				true // bIgnoreSelf
			);
			
			// If no blocking hit, we have line of sight to this part of the tile
			if (!HitResult.bBlockingHit)
			{
				bHasLineOfSight = true;
				break; // Found clear path to at least part of tile, stop testing other endpoints
			}
		}
		
		// If any endpoint had line of sight, add tile to result
		if (bHasLineOfSight)
		{
			TempArray.IntPointArray.Add(TargetPoint);
		}
	}
	
	return TempArray;
}