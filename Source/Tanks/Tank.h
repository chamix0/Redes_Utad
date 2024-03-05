// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "TankMovementComponent.h"
#include "Tank.generated.h"


USTRUCT()
struct FTankState
{
	GENERATED_BODY()

	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	FVector Velocity;

	UPROPERTY()
	FTankMove LastMove;
};


UCLASS()
class TANKS_API ATank : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATank();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class UInputMappingContext* InputMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class UInputConfigData* InputConfigData;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	UPROPERTY(VisibleAnywhere)
	UTankMovementComponent* TankMovementComponent;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* BoxCollider;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Body;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Turret;

	UPROPERTY(EditAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere)
	class UCameraComponent* Camera;
	//
	// UPROPERTY(EditAnywhere)
	// class USpotLightComponent* SpotLight;

	//array of moves
	TArray<FTankMove> UnacknowledgeMoves;


	// void Light();

	void MoveForward(const FInputActionValue& Value);

	void Rotate(const FInputActionValue& Value);


	/*UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveForward(float Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Rotate(float Value);*/

	//Property Replication

	/*UPROPERTY(ReplicatedUsing = OnRep_ReplicatedTransform)
	FTransform ReplicatedTransform;*/

	/*UFUNCTION()
	void OnRep_ReplicatedTransform();*/


	// FVector GetAirResistance();
	//
	// FVector GetRollingResistance();
	//
	//
	// void UpdateLocationFromVelocity(float DeltaTime);
	//
	// void ApplyRotation(float DeltaTime, float Value);


	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FTankMove Move);

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FTankState ServerState;

	UFUNCTION()
	void OnRep_ServerState();
	
	void ClearAcknoledgeMoves(FTankMove lastMove);
};
