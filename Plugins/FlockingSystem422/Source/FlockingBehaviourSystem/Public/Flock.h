// Copyright Ali El Saleh, 2019

#pragma once

#include "GameFramework/Actor.h"
#include "GameFramework/MovementComponent.h"
#include "FlockWorker.h"
#include "Flock.generated.h"

USTRUCT()
struct FBoidSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", DisplayName = "Mesh", meta = (ToolTip = "The static mesh to represent the boid"))
		UStaticMesh* BoidMesh{};

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", DisplayName = "Materials", meta = (ToolTip = "The material/s that matches the boid mesh"))
		TArray<UMaterialInterface*> BoidMaterials{};

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (ClampMin = 0.01f, ClampMax = 1000.0f, ToolTip = "Scale the mesh by the multiplier"))
		float ScaleMultiplier = 1.0f;

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (ToolTip = "The default rotation of the mesh (You will need to experiment which way your mesh should face)"))
		FRotator Rotation = FRotator(90.0f, 180.0f, 0.0f);

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (ToolTip = "Randomize the speed of each boid?"))
		bool bRandomSpeed = false;

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (EditCondition = "!bRandomSpeed", ClampMin = 10.0f, ClampMax = 10000.0f, ToolTip = "The maximum movement speed of each boid"))
		float Speed = 1200.0f;

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (EditCondition = "bRandomSpeed", ClampMin = 100.0f, ClampMax = 1000.0f, ToolTip = "The minimum range, when RandomSpeed is enabled"))
		float MinSpeed = 500.0f;

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (EditCondition = "bRandomSpeed", ClampMin = 1000.0f, ClampMax = 10000.0f, ToolTip = "The maximum range, when RandomSpeed is enabled"))
		float MaxSpeed = 2500.0f;

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (ClampMin = 100.0f, ClampMax = 1000.0f, ToolTip = "How far can each boid see?"))
		float PerceptionRadius = 300.0f;

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (ClampMin = 1.0f, ClampMax = 40.0f, ToolTip = "How fast can each boid turn?"))
		float TurnSpeed = 20.0f;

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (ToolTip = "The distance from the spawn location in Unreal units (cm)", ClampMin = 1000.0f, ClampMax = 100000.0f))
		float WanderDistance = 2000.0f;

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (ToolTip = "Constraint movement to a plane?"))
		bool bConstrainToPlane = false;

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (EditCondition = "bConstrainToPlane", ToolTip = "Restrict movement to a specific axis"))
		EPlaneConstraintAxisSetting PlaneConstraintAxis;
};

USTRUCT()
struct FSpawnSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (ToolTip = "The amount of boids to spawn for this flock", ClampMin = 2, ClampMax = 400))
		uint32 NumberOfBoids = 400;

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (ToolTip = "The spawning area that each boid will be randomly placed in", ClampMin = 100.0f, ClampMax = 2000.0f))
		float SpawnRadius = 1000.0f;
};

USTRUCT()
struct FFlockSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", DisplayName = "Randomize Forces?", meta = (ToolTip = "Randomise all forces at begin play?"))
		bool bRandomForces = false;

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (EditCondition = "!bRandomForces", ClampMin = 0.0f, ClampMax = 10.0f, ToolTip = "How strong is the alignment force? (How easy is it to align with other boids)"))
		float AlignmentForce = 1.0f;

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (EditCondition = "!bRandomForces", ClampMin = 0.0f, ClampMax = 10.0f, ToolTip = "How strong is the cohesion force? (How easy is it to cohere with other boids?)"))
		float CohesionForce = 1.0f;

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (EditCondition = "!bRandomForces", ClampMin = 0.0f, ClampMax = 10.0f, ToolTip = "How strong is the separation force? (How easy is it to separate from other boids?)"))
		float SeparationForce = 1.0f;

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (EditCondition = "!bRandomForces", ClampMin = 0.0f, ClampMax = 3.0f, ToolTip = "How strong is the obstacle avoidance force? (How easy is it to avoid obstacles?)"))
		float AvoidanceForce = 1.0f;

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", DisplayName = "Enable Goal Seeking?", meta = (ToolTip = "Enable/Disable the goal seeking behavior?"))
		bool bEnableGoalSeeking = false;

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", DisplayName = "Goal Actor", meta = (EditCondition = "bEnableGoalSeeking", ToolTip = "The goal actor in the scene that this flock will seek toward."))
		class AGoal* Goal{};

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (EditCondition = "bEnableGoalSeeking", ClampMin = 0.0f, ClampMax = 10.0f, ToolTip = "How strong is the goal seeking force?"))
		float GoalSeekingForce = 1.0f;
};

/*
 A flock is an actor that contains 2 or more boids and will flock with each other when they can see one another.
 */
UCLASS(HideCategories=("Rendering", "Input", "Actor", "LOD", "Cooking", "Replication", "Collision"))
class FLOCKINGBEHAVIOURSYSTEM_API AFlock : public AActor
{
	GENERATED_BODY()

public:
	AFlock();

	void AddBoid();

protected:
	#pragma region Virtual Functions
	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	#endif
	#pragma endregion

	#pragma region Functions
	void ApplyBoidSettings(ABoid* Boid) const;
	void ApplyFlockSettings(ABoid* Boid);

	void CreateNewThread();
	#pragma endregion

	#pragma region Components
		UPROPERTY(EditDefaultsOnly, Category = "Components")
			class USceneComponent* SceneComponent;
		UPROPERTY(EditDefaultsOnly, Category = "Components")
			class UBillboardComponent* BillboardComponent;
	#pragma endregion

	#pragma region Properties
	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (ToolTip = "Customize boid attributes"))
		FBoidSettings BoidSettings;

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (ToolTip = "Specify the spawning rules"))
		FSpawnSettings SpawnSettings;

	UPROPERTY(EditInstanceOnly, Category = "Flock Properties", meta = (ToolTip = "Customize the flocking behavior rules"))
		FFlockSettings FlockSettings;
	#pragma endregion

private:
	// The array of boids
	TArray<class ABoid*> Boids;
	
	// Thread
	FlockWorker* Worker{};

	// The sprite that can be seen in the editor
	UTexture2D* BillboardSprite;
};
