// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TankMovementComponent.h"
#include "Components/ActorComponent.h"
#include "TankMovementReplicatorComponent.generated.h"

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

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TANKS_API UTankMovementReplicatorComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FTankState ServerState;
	UTankMovementComponent* TankMovementComponent;
	TArray<FTankMove> UnacknowledgeMoves;

public:
	// Sets default values for this component's properties
	UTankMovementReplicatorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FTankMove Move);


	UFUNCTION()
	void OnRep_ServerState();

	void ClearAcknoledgeMoves(FTankMove lastMove);
};
