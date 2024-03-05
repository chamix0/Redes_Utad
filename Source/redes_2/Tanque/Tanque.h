// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "redes_2/InputConfigData.h"
#include "Tanque.generated.h"

class UInputMappingContext;

UCLASS()
class REDES_2_API ATanque : public APawn
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	class UBoxComponent* collider; // tank collider

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* bodyMesh; //lower body mesh

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* turretMesh; //turret mesh

	UPROPERTY(EditAnywhere)
	class USpringArmComponent* springArm; //camera spring arm (attached to the turret)

	UPROPERTY(EditAnywhere)
	class UCameraComponent* camera; //camera

	UPROPERTY(EditDefaultsOnly)
	UInputMappingContext* inputMappingContext; //Input map

	UPROPERTY(EditDefaultsOnly)
	UInputConfigData* InputConfigData; //data structure to facilitate getting the Input actions

	//tank values
	UPROPERTY(EditAnywhere)
	float mass = 500; //tank mass in kg
	UPROPERTY(EditAnywhere)
	float maxDrivingForce = 12000;
	UPROPERTY(EditAnywhere)
	float DragCoefficient = 20.f;
	UPROPERTY(EditAnywhere)
	float RollingResistanceCoefficient = 0.15f;
	UPROPERTY(EditAnywhere)
	float maxDegreesPerSecond = 30;
	
	
	//input values
	float Throttle;
	float RotationValue;

	FVector position;
	FVector velocity = FVector(0);

public:
	// Sets default values for this pawn's properties
	ATanque();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void MoveForward(const FInputActionValue& axis);
	void Rotate(const FInputActionValue& value);

	/// calculates the friction caused by the air
	/// @return 
	FVector GetAirResistance() const;

	/// Calculates the coeficient with ground
	/// @return 
	FVector GetRollingResistance() const;
};
