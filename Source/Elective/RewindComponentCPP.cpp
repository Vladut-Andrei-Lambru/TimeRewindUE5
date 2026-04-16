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

	// Cache components so we don’t search every frame
	CacheOwnerReferences();
}

void URewindComponentCPP::CacheOwnerReferences()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	// Get physics component (for cubes)
	CachedPrimitive = Cast<UPrimitiveComponent>(Owner->GetRootComponent());

	// Get movement component (for player)
	if (ACharacter* CharacterOwner = Cast<ACharacter>(Owner))
	{
		CachedCharacterMovement = CharacterOwner->GetCharacterMovement();
	}
}

void URewindComponentCPP::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// RECORDING (time-based, not frame-based)
	if (bIsRecording && !bIsRewinding)
	{
		RecordTimer += DeltaTime;

		// Only record when enough time passed
		while (RecordTimer >= RecordInterval)
		{
			RecordFrame();
			RecordTimer -= RecordInterval;
		}
	}

	// REWINDING
	if (bIsRewinding)
	{
		DoRewindStep();
	}
}

void URewindComponentCPP::RecordFrame()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	FRewindFrame Frame;

	// Save position
	Frame.Transform = Owner->GetActorTransform();

	// Save physics info
	if (CachedPrimitive)
	{
		Frame.LinearVelocity = CachedPrimitive->GetPhysicsLinearVelocity();
		Frame.AngularVelocity = CachedPrimitive->GetPhysicsAngularVelocityInDegrees();
		Frame.bWasSimulatingPhysics = CachedPrimitive->IsSimulatingPhysics();
	}

	// Draw debug path
	if (bDrawDebugPath)
	{
		FVector CurrentLocation = Frame.Transform.GetLocation();

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

	// Store frame
	StoredFrames.Add(Frame);

	// Remove oldest frame if too many
	if (StoredFrames.Num() > MaxStoredFrames)
	{
		StoredFrames.RemoveAt(0);
	}
}

void URewindComponentCPP::StartRewind()
{
	if (bIsRewinding) return;

	bIsRewinding = true;
	bIsRecording = false;
	RecordTimer = 0.0f;

	// Stop player movement
	if (CachedCharacterMovement)
	{
		CachedCharacterMovement->StopMovementImmediately();
		CachedCharacterMovement->DisableMovement();
	}

	// Disable physics so we can control movement manually
	if (CachedPrimitive && CachedPrimitive->IsSimulatingPhysics())
	{
		CachedPrimitive->SetSimulatePhysics(false);
	}
}

void URewindComponentCPP::DoRewindStep()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	for (int i = 0; i < FramesPerRewindStep; i++)
	{
		if (StoredFrames.Num() == 0)
		{
			StopRewind();
			return;
		}

		int32 LastIndex = StoredFrames.Num() - 1;
		FRewindFrame Frame = StoredFrames[LastIndex];

		LastFrame = Frame;

		// Move actor to old position
		Owner->SetActorTransform(Frame.Transform, false, nullptr, ETeleportType::TeleportPhysics);

		StoredFrames.RemoveAt(LastIndex);
	}
}

void URewindComponentCPP::StopRewind()
{
	if (!bIsRewinding) return;

	bIsRewinding = false;
	bIsRecording = true;
	RecordTimer = 0.0f;

	// Restore player movement
	if (CachedCharacterMovement)
	{
		CachedCharacterMovement->StopMovementImmediately();
		CachedCharacterMovement->SetMovementMode(MOVE_Walking);
	}

	// Restore physics
	if (CachedPrimitive)
	{
		CachedPrimitive->SetWorldTransform(LastFrame.Transform, false, nullptr, ETeleportType::TeleportPhysics);

		if (LastFrame.bWasSimulatingPhysics)
		{
			CachedPrimitive->SetSimulatePhysics(true);

			// Restore velocities so motion continues naturally
			CachedPrimitive->SetPhysicsLinearVelocity(LastFrame.LinearVelocity);
			CachedPrimitive->SetPhysicsAngularVelocityInDegrees(LastFrame.AngularVelocity);

			CachedPrimitive->WakeAllRigidBodies();
		}
	}
}

void URewindComponentCPP::ClearStoredFrames()
{
	StoredFrames.Empty();
	bHasPreviousRecordedLocation = false;
}

float URewindComponentCPP::GetStoredTimeSeconds() const
{
	return StoredFrames.Num() * RecordInterval;
}