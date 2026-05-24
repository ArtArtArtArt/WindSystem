// Fill out your copyright notice in the Description page of Project Settings.


#include "WindSubsystem.h"

UE_DISABLE_OPTIMIZATION

static TAutoConsoleVariable<bool> CVarDebugWindDraw(
        TEXT("Sim.Wind.DebugWindDraw"),
        false,
        TEXT("Draw debug wind near the center of the world"),
        ECVF_Default
);

void UWindSubsystem::Tick(float DeltaTime)
{
        if (CVarDebugWindDraw.GetValueOnGameThread())
        {
                DebugDraw(GetWorld());
        }
}

TStatId UWindSubsystem::GetStatId() const
{
        RETURN_QUICK_DECLARE_CYCLE_STAT(
                UWindWorldSubsystem,
                STATGROUP_Tickables
        );
}

FVector UWindSubsystem::GetWindInLocation(const FVector& Location)
{
        constexpr float NoiseScale = 0.0001f;

        float Time = GetWorld()->GetTimeSeconds();

        FVector SamplePos =
                RandomSeedVector
                + Location * NoiseScale +
                FVector(Time * 0.05f);

        // Generate 3 independent noise channels
        float NoiseX = FMath::PerlinNoise3D(
                SamplePos + FVector(12.3f, 45.6f, 78.9f));

        float NoiseY = FMath::PerlinNoise3D(
                SamplePos + FVector(98.7f, 65.4f, 32.1f));

        float NoiseZ = FMath::PerlinNoise3D(
                SamplePos + FVector(11.1f, 22.2f, 33.3f));

        FVector WindDirection =
                FVector(NoiseX, NoiseY, NoiseZ).GetSafeNormal();

        // Strength variation
        float StrengthNoise =
                FMath::PerlinNoise3D(
                        SamplePos + FVector(200.0f)) * 0.1;

        float Strength =
                FMath::Lerp(0.1f, 10.0f,
                        (StrengthNoise + 1.0f) * 0.5f);

        return WindDirection * Strength;
}

void UWindSubsystem::SetWindSeed(int32 Seed)
{
        CurrentSeed = Seed;
        RandomStream.Initialize(CurrentSeed);
        RandomSeedVector = RandomStream.GetUnitVector();
        UE_LOG(LogTemp, Log, TEXT("RandomSeedVector : %f %f %f"), RandomSeedVector.X, RandomSeedVector.Y, RandomSeedVector.Z);
}

void UWindSubsystem::DebugDraw(UWorld* World)
{
        if (!World)
        {
                UE_LOG(LogTemp, Error, TEXT("Wind debug draw failed due to null reference to the World"));
                return;
        }

        constexpr float GridSize = 5000.0f;
        constexpr float CellSize = 200.0f;
        constexpr int32 CellCount = GridSize / CellSize;

        FVector Origin = FVector::ZeroVector;

        //// Draw outer box
        //DrawDebugBox(
        //        World,
        //        Origin,
        //        FVector(GridSize * 0.5f, GridSize * 0.5f, 10.0f),
        //        FColor::White,
        //        false,
        //        0.0f,
        //        0,
        //        2.0f
        //);

        for (int32 X = 0; X < CellCount; ++X)
        {
                for (int32 Y = 0; Y < CellCount; ++Y)
                {
                        for (int32 Z = 0; Z < CellCount; ++Z)
                        {
                                DrawCell(Origin, GridSize, CellSize, X, Y, Z, World);
                        }
                }
        }
}

void UWindSubsystem::DrawCell(FVector& Origin,
        const float GridSize,
        const float CellSize,
        int32 X, int32 Y, int32 Z,
        UWorld* World)
{
        // Cell center
        FVector Location;
        Location.X = Origin.X - GridSize * 0.5f + X * CellSize + CellSize * 0.5f;
        Location.Y = Origin.Y - GridSize * 0.5f + Y * CellSize + CellSize * 0.5f;
        Location.Z = Origin.Z - GridSize * 0.5f + Z * CellSize + CellSize * 0.5f;

        // Get wind vector
        FVector Wind = GetWindInLocation(Location);

        FVector Direction = Wind.GetSafeNormal();

        float WindStrength = Wind.Length() * 10;

        FVector ArrowEnd = Location + Direction * WindStrength;

        DrawDebugDirectionalArrow(
                World,
                Location,
                ArrowEnd,
                30.0f,         // Arrow head size
                FColor(Direction.X*255, Direction.Y * 255, Direction.Z * 255),
                false,
                0.0f,
                0,
                2.0f
        );

        //// Optional: draw cell bounds
        //DrawDebugBox(
        //        World,
        //        Location,
        //        FVector(CellSize * 0.5f),
        //        FColor::White,
        //        false,
        //        0.0f,
        //        0,
        //        0.5f
        //);
}

UE_ENABLE_OPTIMIZATION