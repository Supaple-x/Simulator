// Copyright Ali El Saleh, 2019

#include "Boid.h"
#include "BoidController.h"
#include "Goal.h"
#include "Flock.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "BoidMovementComponent.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"

ABoid::ABoid()
{
	PrimaryActorTick.bCanEverTick = false;

	// Scene component
	SceneComponent = CreateDefaultSubobject<USceneComponent>(FName("SceneComponent"));
	RootComponent = SceneComponent;

	// Static mesh component
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);
	StaticMeshComponent->SetCollisionProfileName(FName("NoCollision"));

	// Object loading
	StaticMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("StaticMesh'/Engine/BasicShapes/Cone.Cone'")));
	Material = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, TEXT("Material'/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial'")));

	// Static mesh component settings
	if (StaticMesh)
	{
		StaticMeshComponent->SetStaticMesh(StaticMesh);
		StaticMeshComponent->SetMaterial(0, Material);
	}
	StaticMeshComponent->SetGenerateOverlapEvents(false);
	StaticMeshComponent->SetEnableGravity(false);
	StaticMeshComponent->SetCanEverAffectNavigation(false);
	StaticMeshComponent->SetCastShadow(false);

	// Movement component
	BoidMovementComponent = CreateDefaultSubobject<UBoidMovementComponent>(TEXT("BoidMovementComponent"));
	BoidMovementComponent->UpdatedComponent = RootComponent;

	// Custom default pawn settings
	AIControllerClass = ABoidController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	bUseControllerRotationYaw = true;
}

void ABoid::BeginPlay()
{
	Super::BeginPlay();

	SpawnLocation = GetActorLocation();
	Rotation = GetActorRotation();

	World = GetWorld();

	if (!BoidMovementComponent->bConstrainToPlane)
		BoidMovementComponent->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::Custom);
}

FVector ABoid::SeekTowardsGoal()
{
	// Get the distance between this and the goal location
	const float Distance = FVector::Dist(GetActorLocation(), Behaviors.GoalSeeking.Goal->GetGoalLocation());

	// Are we inside the acceptance radius
	if (Distance < Behaviors.GoalSeeking.Goal->GetAcceptanceRadius())
	{
		if (Behaviors.GoalSeeking.Goal->IsRandomlyGenerating())
		{
			// Go to next random goal
			if (BoidMovementComponent->bConstrainToPlane)
				Behaviors.GoalSeeking.Goal->FindNewLocation(BoidMovementComponent->GetPlaneConstraintAxisSetting());
			else
				Behaviors.GoalSeeking.Goal->FindNewLocation(EPlaneConstraintAxisSetting::Custom);
		}
		else
		{
			// Go to next goal in the array
			Behaviors.GoalSeeking.Goal->NextLocation();
		}
	}

	// Get the steering vector
	FVector Steer = Seek(Behaviors.GoalSeeking.Goal->GetGoalLocation());

	Steer *= bShouldAvoid ? 0.0f : Behaviors.GoalSeeking.Force;

	return Steer;
}

bool ABoid::HasWanderedTooFar()
{
	// If the boid has wandered off too far
	if (FVector::Dist(GetActorLocation(), SpawnLocation) > WanderDistance)
	{
		return true;
	}
	
	// If the boid is in the wandering area
	return FVector::Dist(GetActorLocation(), SpawnLocation) > WanderDistance - 200.0f;
}

void ABoid::CheckForObstacles()
{
	// Raycast information
	FHitResult OutHit;
	bool bHit = false;
	Velocity = GetVelocity();
	Velocity.Normalize();
	const FVector LineEnd = GetActorLocation() + Velocity * 1000.0f;

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);

	// Do the raycasting
	if (bShouldRaycast)
		bHit = GetWorld()->LineTraceSingleByObjectType(OutHit, GetActorLocation(), LineEnd, ECC_WorldStatic, CollisionQueryParams);

	// If we have hit an obstacle
	if (bHit)
	{
		bShouldRaycast = false;

		// The actor that was hit
		Obstacle = OutHit.GetActor();

		// Store the hit location as the avoid location
		AvoidLocation = OutHit.Location;

		if (!bShouldAvoid)
		{
			bShouldAvoid = true;
		}
	}

	// Calculate the distance between this boid and the avoid location. This value is used to determine whether we should keep avoiding or not.
	DistanceFromHitLocation = FVector::Dist(GetActorLocation(), AvoidLocation);

	if (bShouldAvoid)
	{
		// The avoidance force is a lateral direction from the obstacle
		if (Obstacle)
			Avoidance = AvoidLocation - Obstacle->GetActorLocation();

		Avoidance *= Behaviors.Avoidance.Force;

		// Stop avoiding when the avoid location is far away from us. (In this case, 100 units)
		const bool bStopAvoiding = DistanceFromHitLocation > 100.0f;
		if (bStopAvoiding)
		{
			// Stop avoiding and move back into the flock
			bShouldAvoid = false;
			bShouldRaycast = true;

			Avoidance = FVector::ZeroVector;
		}
	}
}

void ABoid::ApplyBehaviors(TArray<ABoid*>& Boids)
{
	// Seek towards a goal
	if (Behaviors.GoalSeeking.bEnabled && Behaviors.GoalSeeking.Goal)
		GoalSeeking = SeekTowardsGoal();

	// Apply the three flocking rules when we can see other boids
	Alignment = Align(Boids);
	Cohesion = Cohere(Boids);
	Separation = Separate(Boids);

	CheckForObstacles();

	// Weights
	Alignment *= bShouldAvoid ? 0.0f : Behaviors.Alignment.Force;
	Cohesion *= bShouldAvoid ? 0.0f : Behaviors.Cohesion.Force;
	Separation *= bShouldAvoid ? 0.0f : Behaviors.Separation.Force;

	// Add all the forces together to produce the flocking vector
	ApplyForces();
}

void ABoid::ApplyForces()
{
	Flocking = Alignment + Cohesion + Separation;
	
	// Are we wandering?
	if (!Behaviors.GoalSeeking.bEnabled)
	{
		// Return back if we've wandered off too far
		if (HasWanderedTooFar())
		{
			const FVector Direction = SpawnLocation - GetActorLocation();
			Flocking = Direction + GetVelocity();
		}
	}
	else
	{
		Flocking += GoalSeeking;
	}

	if (bShouldAvoid)
		Flocking = Avoidance;
	
	// Apply the final flocking vector
	ApplyForce(Flocking);
}

void ABoid::ApplyForce(const FVector& Force)
{
	// Calculate the new steering force if force is not an invalid number
	if (!Force.ContainsNaN())
	{
		const FVector Steering = Force + GetVelocity();

		BoidMovementComponent->AddInputVector(Steering);
		
		CalculateRotation(Steering);
	}
}

FVector ABoid::Align(TArray<ABoid*>& Boids) const
{
	FVector SumOfVectors; // Used for calculating the average velocity
	const float Radius = PerceptionRadius; // The value of how far the boid see
	unsigned short Neighbors = 0; // Used to count how many neighbors (boids) there are within its perception radius

	// Loop through every single boid
	for (auto Other : Boids)
	{
		// Check their distances between each other
		const float Distance = FVector::Dist(GetActorLocation(), Other->GetActorLocation());

		// If they are within the boids perception
		if (Distance > 0 && Distance < Radius)
		{
			// Add up their velocities and add 1 to the amount of neighbors found
			SumOfVectors += Other->GetVelocity();
			Neighbors++;
		}
	}

	// Only calculate the steering force if there is at least 1 neighbor
	if (Neighbors > 0)
	{
		SumOfVectors /= Neighbors; // Get the average velocity vectors

		SumOfVectors.Normalize();
		SumOfVectors *= BoidMovementComponent->GetMaxSpeed();

		FVector Steer = SumOfVectors + GetVelocity(); // Reynold's steering formula

		if (Steer.ContainsNaN())
			Steer = GetVelocity();

		return Steer;
	}

	return FVector(0.0f);
}

FVector ABoid::Cohere(TArray<ABoid*>& Boids)
{
	FVector SumOfVectors; // Used for calculating the average location
	const float Radius = PerceptionRadius; // The value of how far the boid see
	unsigned short Neighbors = 0; // Used to count how many neighbors (boids) there are within its perception radius

	// Loop through every single boid
	for (auto Other : Boids)
	{
		// Check their distances between each other
		const float Distance = FVector::Dist(GetActorLocation(), Other->GetActorLocation());

		// If they are within the boids perception
		if (Distance > 0 && Distance < Radius)
		{
			// Add up their locations and add 1 to the amount of neighbors found
			SumOfVectors += Other->GetActorLocation();
			Neighbors++;
		}
	}

	// Only seek toward the average location if there is at least 1 neighbor
	if (Neighbors > 0)
	{
		SumOfVectors /= Neighbors; // Get the average location

		SumOfVectors.Normalize();
		SumOfVectors *= BoidMovementComponent->GetMaxSpeed();

		return Seek(SumOfVectors);
	}

	return FVector(0.0f);
}

FVector ABoid::Separate(TArray<ABoid*>& Boids) const
{
	FVector SumOfVectors; // Used for calculating the average location
	const float DesiredSeparation = PerceptionRadius - 100.0f; // The value of how far the boid see
	unsigned short Neighbors = 0; // Used to count how many neighbors (boids) there are within its perception radius

	// Loop through every single boid
	for (auto Other : Boids)
	{
		// Check their distances between each other
		const float Distance = FVector::Dist(GetActorLocation(), Other->GetActorLocation());

		// If they are within the boids perception
		if (Distance > 0 && Distance < DesiredSeparation)
		{
			// Calculate the difference between each other and add 1 to the amount of neighbors found
			FVector Difference = GetActorLocation() - Other->GetActorLocation();

			Difference.Normalize();
			Difference /= Distance;
			SumOfVectors += Difference;

			Neighbors++;
		}
	}

	// Only calculate the steering force if there is at least 1 neighbor
	if (Neighbors > 0)
	{
		SumOfVectors /= Neighbors; // Get the average location

		SumOfVectors.Normalize();
		SumOfVectors *= BoidMovementComponent->GetMaxSpeed();

		// Reynold's steering formula
		FVector Steer = SumOfVectors + GetVelocity();

		if (Steer.ContainsNaN())
			Steer = GetVelocity();

		return Steer;
	}

	return FVector(0.0f);
}

FVector ABoid::Seek(const FVector& Target)
{
	// Get the direction to the target
	const FVector Direction = Target - GetActorLocation();

	// Reynold's Steering formula
	FVector Steer = Direction + GetVelocity();

	if (Steer.ContainsNaN() || Steer.IsZero())
		Steer = GetVelocity();

	return Steer;
}

void ABoid::AssignMesh(UStaticMesh* NewMesh, TArray<UMaterialInterface*> Materials) const
{
	if (NewMesh)
	{
		StaticMeshComponent->SetStaticMesh(NewMesh);

		for (int32 i = 0; i < Materials.Num(); i++)
			StaticMeshComponent->SetMaterial(i, Materials[i]);
	}
}

void ABoid::SetMeshScale(const float NewScaleMultiplier) const
{
	StaticMeshComponent->SetWorldScale3D(FVector(NewScaleMultiplier));
}

void ABoid::SetMeshRotation(const FRotator NewRotation) const
{
	StaticMeshComponent->SetRelativeRotation(NewRotation);
}

float ABoid::GetPerceptionRadius() const
{
	return PerceptionRadius;
}

void ABoid::SetSpeed(const float NewSpeed) const
{
	BoidMovementComponent->MaxSpeed = NewSpeed;
}

void ABoid::SetWanderDistance(const float NewWanderDistance)
{
	WanderDistance = NewWanderDistance;
}

void ABoid::SetGoal(AGoal* NewGoal)
{
	Behaviors.GoalSeeking.Goal = NewGoal;
}

void ABoid::SetFlock(AFlock* NewFlock)
{
	OwnedBy = NewFlock;
}

void ABoid::EnablePlaneConstraint(const bool bValue)
{
	BoidMovementComponent->SetPlaneConstraintEnabled(bValue);
}

void ABoid::SetPlaneConstraintAxis(const EPlaneConstraintAxisSetting NewPlaneConstraintAxis)
{
	if (!BoidMovementComponent->bConstrainToPlane)
		BoidMovementComponent->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::Custom);
	else
		BoidMovementComponent->SetPlaneConstraintAxisSetting(NewPlaneConstraintAxis);
}

void ABoid::CalculateRotation(FVector Steering)
{
	Rotation = Steering.Rotation();
}

void ABoid::UpdateRotation(const float DeltaTime)
{
	SetActorRotation(FMath::Lerp(GetActorRotation(), Rotation, TurnSpeed * DeltaTime));
}

void ABoid::EnableGoalSeeking(const bool bValue)
{
	Behaviors.GoalSeeking.bEnabled = bValue;
}

void ABoid::Debug()
{
	//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + Velocity * 1000.0f, FColor::Red);
	DrawDebugLine(GetWorld(), AvoidLocation, Avoidance, FColor::Green);
	DrawDebugPoint(GetWorld(), AvoidLocation, 10.0f, FColor::Red);
}

void ABoid::SetAlignmentForce(const float ForceMultiplier)
{
	Behaviors.Alignment.Force = ForceMultiplier;
}

void ABoid::SetCohesionForce(const float ForceMultiplier)
{
	Behaviors.Cohesion.Force = ForceMultiplier;
}

void ABoid::SetSeparationForce(const float ForceMultiplier)
{
	Behaviors.Separation.Force = ForceMultiplier;
}

void ABoid::SetGoalSeekingForce(const float ForceMultiplier)
{
	Behaviors.GoalSeeking.Force = ForceMultiplier;
}

void ABoid::SetAvoidanceForce(const float ForceMultiplier)
{
	Behaviors.Avoidance.Force = ForceMultiplier;
}

void ABoid::SetPerceptionRadius(const float NewPerceptionRadius)
{
	PerceptionRadius = NewPerceptionRadius;
}

void ABoid::SetTurnSpeed(const float NewTurnSpeed)
{
	TurnSpeed = NewTurnSpeed;
}

float ABoid::GetSpeed() const
{
	return BoidMovementComponent->GetMaxSpeed();
}

float ABoid::GetTurnSpeed() const
{
	return TurnSpeed;
}
