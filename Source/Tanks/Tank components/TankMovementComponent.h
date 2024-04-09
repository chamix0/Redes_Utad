// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TankMovementComponent.generated.h"


USTRUCT()
struct FTankMove
{
	GENERATED_BODY()

	UPROPERTY()
	float Throttle;

	UPROPERTY()
	float RotationValue;

	UPROPERTY()
	float TurretRotationValue;

	UPROPERTY()
	float DeltaTime;

	UPROPERTY()
	float Time;

	UPROPERTY()
	bool LightOn;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TANKS_API UTankMovementComponent : public UActorComponent
{
	GENERATED_BODY()

	//data
public:

private:
	//Mass of the tank (kg)
	UPROPERTY(EditAnywhere)
	float Mass = 5000;

	//Force applied when the throttle fully down (N newtons)
	UPROPERTY(EditAnywhere)
	float MaxDrivingForce = 12000;

	//Higher means more drag
	UPROPERTY(EditAnywhere)
	float DragCoefficient = 20;

	UPROPERTY(EditAnywhere)
	float RollingResistanceCoefficient = 0.15;

	UPROPERTY(EditAnywhere)
	float MaxDegreesPerSecond = 30;

	UPROPERTY(EditAnywhere)
	float MinTurningRadius = 5;

	UPROPERTY()
	float Throttle;

	UPROPERTY()
	float RotationValue;

	UPROPERTY()
	float TurretRotationValue;

	UPROPERTY()
	FVector Velocity;

public:
	// Sets default values for this component's properties
	UTankMovementComponent();

	void SimulateMove(const FTankMove& Move);

	FTankMove CreateMove(float DeltaTime);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	float GetThrottle() const;
	void SetThrottle(float value);
	FVector GetVelocity() const;
	void SetVelocity(FVector value);
	float GetRotationValue() const;
	void SetRotationValue(float value);
	float GetTurretRotationValue() const;
	void SetTurretRotationValue(float value);

private:
	FVector GetAirResistance();

	FVector GetRollingResistance();

	void UpdateLocationFromVelocity(float DeltaTime);

	void ApplyRotation(float DeltaTime, float Value);
	void ApplyTurretRotation(float DeltaTime, float Value);

};
