// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WindSubsystem.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class WINDSYSTEM_API UWindSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	UFUNCTION(BlueprintCallable, Category="WindSubsystem")
	FVector GetWindInLocation(const FVector& Location);

	UFUNCTION(BlueprintCallable)
	void SetWindSeed(int32 Seed);

protected:
	FRandomStream RandomStream;
	FVector RandomSeedVector;
	int32 CurrentSeed = 12345;

private:
	void DebugDraw(UWorld* World);
	void DrawCell(FVector& Origin, 
		const float GridSize,
		const float CellSize,  
		int32 X, int32 Y, int32 Z, 
		UWorld* World);
};
