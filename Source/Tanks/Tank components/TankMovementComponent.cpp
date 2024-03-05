// Fill out your copyright notice in the Description page of Project Settings.


#include "TankMovementComponent.h"

#include "../TanksGameModeBase.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UTankMovementComponent::UTankMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UTankMovementComponent::SimulateMove(const FTankMove& Move)
{
	//Fuerza = dirección * fuerza máxima * acelerador
	FVector Force = GetOwner()->GetActorForwardVector() * MaxDrivingForce * Move.Throttle;

	//Air Resistance
	Force += GetAirResistance();

	//Rolling Resistance
	Force += GetRollingResistance();

	//Aceleración = fuerza / masa
	FVector Acceleration = Force / Mass;

	//velocidad = aceleración * tiempo
	Velocity += Acceleration * Move.DeltaTime;

	UpdateLocationFromVelocity(Move.DeltaTime);

	ApplyRotation(Move.DeltaTime, Move.RotationValue);

	// SpotLight->SetVisibility(Move.LightOn);
}

FTankMove UTankMovementComponent::CreateMove(float DeltaTime)
{
	FTankMove Move;
	Move.DeltaTime = DeltaTime;
	Move.RotationValue = RotationValue;
	Move.Throttle = Throttle;
	Move.Time = GetWorld()->TimeSeconds;

	// Move.LightOn = SpotLight->IsVisible();

	return Move;
}


// Called when the game starts
void UTankMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UTankMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UTankMovementComponent::GetThrottle() const
{
	return Throttle;
}

void UTankMovementComponent::SetThrottle(float value)
{
	Throttle = value;
}

FVector UTankMovementComponent::GetVelocity() const
{
	return Velocity;
}

void UTankMovementComponent::SetVelocity(FVector value)
{
	Velocity = value;
}

float UTankMovementComponent::GetRotationValue() const
{
	return RotationValue;
}

void UTankMovementComponent::SetRotationValue(float value)
{
	RotationValue = value;
}

FVector UTankMovementComponent::GetAirResistance()
{
	return -Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient;
}

FVector UTankMovementComponent::GetRollingResistance()
{
	float AccelerationForGravity = -GetWorld()->GetGravityZ() / 100;
	float NormalForce = Mass * AccelerationForGravity;
	return -Velocity.GetSafeNormal() * RollingResistanceCoefficient * NormalForce;
}


void UTankMovementComponent::UpdateLocationFromVelocity(float DeltaTime)
{
	FVector Translation = Velocity * 100 * DeltaTime;

	GetOwner()->AddActorWorldOffset(Translation, true);

	FHitResult Hit;
	GetOwner()->AddActorWorldOffset(Translation, true, &Hit);

	if (Hit.IsValidBlockingHit())
	{
		Velocity = FVector::ZeroVector;
	}
}

void UTankMovementComponent::ApplyRotation(float DeltaTime, float Value)
{
	float RotationAngle = MaxDegreesPerSecond * DeltaTime * Value;

	float dir = FVector::DotProduct(GetOwner()->GetActorForwardVector(), Velocity.GetSafeNormal());

	if (Velocity.Size() > 0.1 && dir < 0)
	{
		RotationAngle *= -1;
	}

	FQuat RotationDelta(GetOwner()->GetActorUpVector(), FMath::DegreesToRadians(RotationAngle));
	Velocity = RotationDelta.RotateVector(Velocity);
	GetOwner()->AddActorWorldRotation(RotationDelta);
}
