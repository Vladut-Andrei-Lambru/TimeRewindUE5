#include "RewindComponentCPP.h"

#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

URewindComponentCPP::URewindComponentCPP()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URewindComponentCPP::BeginPlay()
{
	Super::BeginPlay();

	// Cache references once at the start
	CacheOwnerReferences();
}

void URewindComponentCPP::CacheOwnerReferences()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	// For physics actors like cubes
	CachedPrimitive = Cast<UPrimitiveComponent>(Owner->GetRootComponent());

	// For character actors like the player
	if (ACharacter* CharacterOwner = Cast<ACharacter>(Owner))
	{
		CachedCharacterMovement = CharacterOwner->GetCharacterMovement();
	}
}

void URewindComponentCPP::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Record frames at a fixed time interval
	if (bIsRecording && !bIsRewinding)
	{
		RecordTimer += DeltaTime;

		while (RecordTimer >= RecordInterval)
		{
			RecordFrame();
			RecordTimer -= RecordInterval;
		}
	}

	// Rewind frames backwards
	if (bIsRewinding)
	{
		DoRewindStep();
	}
}

void URewindComponentCPP::RecordFrame()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	FRewindFrame Frame;

	// Save actor transform
	Frame.Transform = Owner->GetActorTransform();

	// Save physics info if this actor has a primitive root
	if (CachedPrimitive)
	{
		Frame.LinearVelocity = CachedPrimitive->GetPhysicsLinearVelocity();
		Frame.AngularVelocity = CachedPrimitive->GetPhysicsAngularVelocityInDegrees();
		Frame.bWasSimulatingPhysics = CachedPrimitive->IsSimulatingPhysics();
	}

	// Optional debug path drawing
	if (bDrawDebugPath)
	{
		const FVector CurrentLocation = Frame.Transform.GetLocation();

		if (bHasPreviousRecordedLocation)
		{
			DrawDebugLine(
				GetWorld(),
				PreviousRecordedLocation,
				CurrentLocation,
				DebugLineColor,
				false,
				DebugLineLifetime,
				0,
				2.0f
			);
		}

		PreviousRecordedLocation = CurrentLocation;
		bHasPreviousRecordedLocation = true;
	}

	StoredFrames.Add(Frame);

	// Keep array size under the max
	if (StoredFrames.Num() > MaxStoredFrames)
	{
		StoredFrames.RemoveAt(0);
	}
}

void URewindComponentCPP::StartRewind()
{
	if (bIsRewinding)
	{
		return;
	}

	bIsRewinding = true;
	bIsRecording = false;
	RecordTimer = 0.0f;

	// Stop player movement while rewinding
	if (CachedCharacterMovement)
	{
		CachedCharacterMovement->StopMovementImmediately();
		CachedCharacterMovement->DisableMovement();
	}

	// Disable physics so the rewind can control movement directly
	if (CachedPrimitive && CachedPrimitive->IsSimulatingPhysics())
	{
		CachedPrimitive->SetSimulatePhysics(false);
	}

	OnRewindStartedBP();
}

void URewindComponentCPP::DoRewindStep()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	for (int32 i = 0; i < FramesPerRewindStep; i++)
	{
		if (StoredFrames.Num() == 0)
		{
			StopRewind();
			return;
		}

		const int32 LastIndex = StoredFrames.Num() - 1;
		const FRewindFrame Frame = StoredFrames[LastIndex];

		LastFrame = Frame;

		// Apply the stored transform exactly
		Owner->SetActorTransform(Frame.Transform, false, nullptr, ETeleportType::TeleportPhysics);

		StoredFrames.RemoveAt(LastIndex);
	}
}

void URewindComponentCPP::StopRewind()
{
	if (!bIsRewinding)
	{
		return;
	}

	bIsRewinding = false;
	bIsRecording = true;
	RecordTimer = 0.0f;

	// Restore character movement
	if (CachedCharacterMovement)
	{
		CachedCharacterMovement->StopMovementImmediately();
		CachedCharacterMovement->SetMovementMode(MOVE_Walking);
	}

	// Restore physics state
	if (CachedPrimitive)
	{
		CachedPrimitive->SetWorldTransform(LastFrame.Transform, false, nullptr, ETeleportType::TeleportPhysics);

		if (LastFrame.bWasSimulatingPhysics)
		{
			CachedPrimitive->SetSimulatePhysics(true);
			CachedPrimitive->SetPhysicsLinearVelocity(LastFrame.LinearVelocity);
			CachedPrimitive->SetPhysicsAngularVelocityInDegrees(LastFrame.AngularVelocity);
			CachedPrimitive->WakeAllRigidBodies();
		}
	}

	OnRewindStoppedBP();
}

void URewindComponentCPP::ClearStoredFrames()
{
	StoredFrames.Empty();
	bHasPreviousRecordedLocation = false;
	PreviousRecordedLocation = FVector::ZeroVector;
}

float URewindComponentCPP::GetStoredTimeSeconds() const
{
	return StoredFrames.Num() * RecordInterval;
}