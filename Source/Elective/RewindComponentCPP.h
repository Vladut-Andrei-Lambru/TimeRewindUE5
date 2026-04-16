#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RewindTypes.h"
#include "RewindComponentCPP.generated.h"

class UPrimitiveComponent;
class UCharacterMovementComponent;

// This component handles recording and rewinding time
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ELECTIVE_API URewindComponentCPP : public UActorComponent
{
	GENERATED_BODY()

public:
	URewindComponentCPP();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// === RECORDING SETTINGS ===

	// How many frames we store
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewind|Recording")
	int32 MaxStoredFrames = 300;

	// Time between each recorded frame (IMPORTANT for consistent rewind time)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewind|Recording")
	float RecordInterval = 0.02f;

	// === REWIND SETTINGS ===

	// How many frames we rewind per tick (controls speed)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewind|Playback")
	int32 FramesPerRewindStep = 1;

	// === DEBUG ===

	// Draw lines to show movement path
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewind|Debug")
	bool bDrawDebugPath = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewind|Debug")
	float DebugLineLifetime = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewind|Debug")
	FColor DebugLineColor = FColor::Cyan;

	// === STATE ===

	// Are we currently recording time
	UPROPERTY(BlueprintReadOnly, Category = "Rewind")
	bool bIsRecording = true;

	// Are we currently rewinding
	UPROPERTY(BlueprintReadOnly, Category = "Rewind")
	bool bIsRewinding = false;

	// Stored frames
	UPROPERTY(BlueprintReadOnly, Category = "Rewind")
	TArray<FRewindFrame> StoredFrames;

	// Last frame used (important for restoring physics)
	UPROPERTY(BlueprintReadOnly, Category = "Rewind")
	FRewindFrame LastFrame;

	// === FUNCTIONS ===

	// Start rewinding
	UFUNCTION(BlueprintCallable, Category = "Rewind")
	void StartRewind();

	// Stop rewinding
	UFUNCTION(BlueprintCallable, Category = "Rewind")
	void StopRewind();

	// Clear all stored data
	UFUNCTION(BlueprintCallable, Category = "Rewind")
	void ClearStoredFrames();

	// Get how many seconds are stored
	UFUNCTION(BlueprintPure, Category = "Rewind")
	float GetStoredTimeSeconds() const;

private:
	void RecordFrame();
	void DoRewindStep();
	void CacheOwnerReferences();

	UPROPERTY()
	UPrimitiveComponent* CachedPrimitive = nullptr;

	UPROPERTY()
	UCharacterMovementComponent* CachedCharacterMovement = nullptr;

	// Used for drawing debug path
	FVector PreviousRecordedLocation = FVector::ZeroVector;
	bool bHasPreviousRecordedLocation = false;

	// Timer for fixed recording interval
	float RecordTimer = 0.0f;
};