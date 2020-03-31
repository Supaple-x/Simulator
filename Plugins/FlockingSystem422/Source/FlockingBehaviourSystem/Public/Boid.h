// Copyright Ali El Saleh, 2019

#pragma once

#include "GameFramework/Pawn.h"
#include "GameFramework/MovementComponent.h"
#include "Boid.generated.h"


USTRUCT()
struct FDebugVariables_Boid
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditInstanceOnly, Category = "Debug", meta = (ToolTip = "Visualize the boid's perception radius?"))
		bool bShowPerceptionRadius = false;

	UPROPERTY(EditInstanceOnly, Category = "Debug", meta = (ToolTip = "Visualize the found boid?"))
		bool bShowFoundBoid = false;

	UPROPERTY(EditInstanceOnly, Category = "Debug", meta = (ToolTip = "Visualize the wander behavior?"))
		bool bShowWanderInfo = false;

	UPROPERTY(EditInstanceOnly, Category = "Debug", meta = (ToolTip = "Visualize the movement path?"))
		bool bShowMovementTrail = false;

	UPROPERTY(EditInstanceOnly, Category = "Debug", meta = (ToolTip = "Visualize the velocity vector?"))
		bool bShowVelocityVector = false;

	UPROPERTY(EditInstanceOnly, Category = "Debug", meta = (ToolTip = "Visualize the desired velocity vector?"))
		bool bShowDesiredVelocityVector = false;

	UPROPERTY(EditInstanceOnly, Category = "Debug", meta = (ToolTip = "Visualize the steering vector?"))
		bool bShowSteeringVector = false;
};

USTRUCT()
struct FGoalSeeking
{
	GENERATED_USTRUCT_BODY()

	// Enable/Disable the goal seeking behavior?
	bool bEnabled = false;

	// The goal actor in the scene that this boid will seek toward.
	class AGoal* Goal{};

	// How strong is the goal seeking force?
	float Force = 1.0f;
};

USTRUCT()
struct FAlignment
{
	GENERATED_USTRUCT_BODY()

	// How strong is the alignment force? (How easy is it to align with other boids)
	float Force = 1.0f;
};

USTRUCT()
struct FCohesion
{
	GENERATED_USTRUCT_BODY()

	// How strong is the cohesion force? (How easy is it to cohere with other boids?)
	float Force = 1.0f;
};

USTRUCT()
struct FSeparation
{
	GENERATED_USTRUCT_BODY()

	// How strong is the separation force? (How easy is it to separate from other boids?)
	float Force = 1.0f;
};

USTRUCT()
struct FAvoidance
{
	GENERATED_USTRUCT_BODY()

	// How strong is the obstacle avoidance force? (How easy is it to avoid obstacles?)
	float Force = 1.0f;
};

USTRUCT()
struct FBehaviors
{
	GENERATED_USTRUCT_BODY()

	FAlignment Alignment;
	FCohesion Cohesion;
	FSeparation Separation;
	FAvoidance Avoidance;
	FGoalSeeking GoalSeeking;
};


UCLASS(HideCategories = ("Actor", "Cooking", "LOD", "Input", "Pawn", "Camera", "Replication", "Collision"))
class FLOCKINGBEHAVIOURSYSTEM_API ABoid : public APawn
{
	GENERATED_BODY()

public:
	ABoid();

	#pragma region Flocking
	void ApplyBehaviors(TArray<ABoid*>& Boids);
	void CalculateRotation(FVector Steering);
	void UpdateRotation(float DeltaTime);
	#pragma endregion

	#pragma region Getters
	float GetSpeed() const;
	float GetTurnSpeed() const;
	float GetPerceptionRadius() const;
	#pragma endregion

	#pragma region Setters
	void AssignMesh(UStaticMesh* NewMesh, TArray<UMaterialInterface*> Materials) const;
	void SetMeshRotation(FRotator NewRotation) const;
	void SetMeshScale(float NewScaleMultiplier) const;
	void SetAlignmentForce(float ForceMultiplier);
	void SetCohesionForce(float ForceMultiplier);
	void SetSeparationForce(float ForceMultiplier);
	void SetGoalSeekingForce(float ForceMultiplier);
	void SetAvoidanceForce(float ForceMultiplier);
	void SetPerceptionRadius(float NewPerceptionRadius);
	void SetSpeed(float NewSpeed) const;
	void SetTurnSpeed(float NewTurnSpeed);
	void SetWanderDistance(float NewWanderDistance);
	void SetGoal(class AGoal* NewGoal);
	void SetFlock(class AFlock* NewFlock);
	void EnablePlaneConstraint(bool bValue);
	void SetPlaneConstraintAxis(EPlaneConstraintAxisSetting NewPlaneConstraintAxis);
	void EnableGoalSeeking(bool bValue);
	#pragma endregion

	void Debug();

protected:
	void BeginPlay() override;

	#pragma region Functions
	FVector Seek(const FVector& Target);
	FVector SeekTowardsGoal();
	FVector Align(TArray<ABoid*>& Boids) const;
	FVector Cohere(TArray<ABoid*>& Boids);
	FVector Separate(TArray<ABoid*>& Boids) const;

	bool HasWanderedTooFar();
	void CheckForObstacles();

	void ApplyForce(const FVector& Force);
	void ApplyForces();
	#pragma endregion

	#pragma region Components
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		USceneComponent* SceneComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UBoidMovementComponent* BoidMovementComponent{};
	#pragma endregion

	#pragma region Properties
	UPROPERTY(VisibleAnywhere, Category = "Properties")
		class AFlock* OwnedBy{};

	FBehaviors Behaviors;
	#pragma endregion

private:
	class ABoidController* BoidController{};

	UWorld* World{}; // Pointer to the cached world
	UStaticMesh* StaticMesh{};
	UMaterialInterface* Material;

	// Forces
	FVector Flocking;
	FVector Alignment;
	FVector Cohesion;
	FVector Separation;
	FVector GoalSeeking;
	FVector Avoidance;

	FVector SpawnLocation;
	FRotator Rotation;
	FVector AvoidLocation;
	FVector Velocity;
	AActor* Obstacle;

	float DistanceFromHitLocation = 0.0f;		
	float WanderDistance = 2000.0f; // The distance from the spawn location
	float PerceptionRadius = 300.0f;
	float TurnSpeed = 20.0f;

	bool bShouldRaycast = true;
	bool bShouldAvoid = false;
};
