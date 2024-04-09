// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "./Tank components/TankMovementComponent.h"
#include "Tank components/TankMovementReplicatorComponent.h"
#include "Tank.generated.h"


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
	UStaticMeshComponent* GetTurret() const;

private:
	UPROPERTY(VisibleAnywhere)
	UTankMovementComponent* TankMovementComponent;

	UPROPERTY(VisibleAnywhere)
	UTankMovementReplicatorComponent* TankMovementReplicatorComponent;

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


	//array of moves
	TArray<FTankMove> UnacknowledgeMoves;

	// void Light();
	void MoveForward(const FInputActionValue& Value);
	void Rotate(const FInputActionValue& Value);
	void RotateTurret(const FInputActionValue& Value);
};
