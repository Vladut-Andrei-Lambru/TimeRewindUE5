#pragma once

#include "CoreMinimal.h"
#include "RewindTypes.generated.h"

// This struct stores one frame of rewind data
// It saves position + physics info
USTRUCT(BlueprintType)
struct FRewindFrame
{
	GENERATED_BODY()

	// Where the actor was
	UPROPERTY(BlueprintReadWrite)
	FTransform Transform;

	// How fast it was moving
	UPROPERTY(BlueprintReadWrite)
	FVector LinearVelocity = FVector::ZeroVector;

	// How fast it was rotating
	UPROPERTY(BlueprintReadWrite)
	FVector AngularVelocity = FVector::ZeroVector;

	// Was physics turned on at this moment
	UPROPERTY(BlueprintReadWrite)
	bool bWasSimulatingPhysics = false;
};