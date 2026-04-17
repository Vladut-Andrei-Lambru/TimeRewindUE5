#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RewindTypes.h"
#include "RewindComponentCPP.generated.h"

class UPrimitiveComponent;
class UCharacterMovementComponent;

// This component records movement and rewinds it later
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ELECTIVE_API URewindComponentCPP : public UActorComponent
{
	GENERATED_BODY()

public:
	URewindComponentCPP();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

	// -----------------------------
	// RECORDING SETTINGS
	// -----------------------------

	// How many stored frames are kept
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewind|Recording")
	int32 MaxStoredFrames = 300;

	// Time between each recorded frame
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewind|Recording")
	float RecordInterval = 0.02f;

	// -----------------------------
	// REWIND SETTINGS
	// -----------------------------

	// How many frames are applied each tick while rewinding
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewind|Playback")
	int32 FramesPerRewindStep = 1;

	// -----------------------------
	// DEBUG
	// -----------------------------

	// Draw debug line path while recording
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewind|Debug")
	bool bDrawDebugPath = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewind|Debug")
	float DebugLineLifetime = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewind|Debug")
	FColor DebugLineColor = FColor::Cyan;

	// -----------------------------
	// STATE
	// -----------------------------

	UPROPERTY(BlueprintReadOnly, Category = "Rewind")
	bool bIsRecording = true;

	UPROPERTY(BlueprintReadOnly, Category = "Rewind")
	bool bIsRewinding = false;

	UPROPERTY(BlueprintReadOnly, Category = "Rewind")
	TArray<FRewindFrame> StoredFrames;

	UPROPERTY(BlueprintReadOnly, Category = "Rewind")
	FRewindFrame LastFrame;

	// -----------------------------
	// FUNCTIONS
	// -----------------------------

	UFUNCTION(BlueprintCallable, Category = "Rewind")
	void StartRewind();

	UFUNCTION(BlueprintCallable, Category = "Rewind")
	void StopRewind();

	UFUNCTION(BlueprintCallable, Category = "Rewind")
	void ClearStoredFrames();

	UFUNCTION(BlueprintPure, Category = "Rewind")
	float GetStoredTimeSeconds() const;

	// These can be used in Blueprint for particles / sounds / trails
	UFUNCTION(BlueprintImplementableEvent, Category = "Rewind")
	void OnRewindStartedBP();

	UFUNCTION(BlueprintImplementableEvent, Category = "Rewind")
	void OnRewindStoppedBP();

private:
	void RecordFrame();
	void DoRewindStep();
	void CacheOwnerReferences();

	UPROPERTY()
	UPrimitiveComponent* CachedPrimitive = nullptr;

	UPROPERTY()
	UCharacterMovementComponent* CachedCharacterMovement = nullptr;

	// Used for debug path drawing
	FVector PreviousRecordedLocation = FVector::ZeroVector;
	bool bHasPreviousRecordedLocation = false;

	// Used for fixed time recording
	float RecordTimer = 0.0f;
};