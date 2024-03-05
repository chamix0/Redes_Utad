// Fill out your copyright notice in the Description page of Project Settings.


#include "TankMovementReplicatorComponent.h"

#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UTankMovementReplicatorComponent::UTankMovementReplicatorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);

	// ...
}


// Called when the game starts
void UTankMovementReplicatorComponent::BeginPlay()
{
	Super::BeginPlay();

	TankMovementComponent = GetOwner()->GetComponentByClass<UTankMovementComponent>();
}


// Called every frame
void UTankMovementReplicatorComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (TankMovementComponent != nullptr)
		{
			FTankMove Move = TankMovementComponent->CreateMove(DeltaTime);
			TankMovementComponent->SimulateMove(Move);
			Server_SendMove(Move);
		}
	}
	APawn* owner = Cast<APawn>(GetOwner());
	if (GetOwner()->GetLocalRole() == ROLE_Authority && owner != nullptr && owner->IsLocallyControlled())
	{
		if (TankMovementComponent != nullptr)
		{
			FTankMove Move = TankMovementComponent->CreateMove(DeltaTime);
			Server_SendMove(Move);
		}
	}

	if (GetOwner()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		if (TankMovementComponent != nullptr)
		{
			TankMovementComponent->SimulateMove(ServerState.LastMove);
		}
	}

	// ...
}

void UTankMovementReplicatorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTankMovementReplicatorComponent, ServerState)
	/*DOREPLIFETIME(ATank, ReplicatedTransform);
	DOREPLIFETIME(ATank, Throttle);
	DOREPLIFETIME(ATank, RotationValue);
	DOREPLIFETIME(ATank, Velocity);*/
}

void UTankMovementReplicatorComponent::OnRep_ServerState()
{
	if (TankMovementComponent != nullptr)
	{
		GetOwner()->SetActorTransform(ServerState.Transform);
		TankMovementComponent->SetVelocity(ServerState.Velocity);

		ClearAcknoledgeMoves(ServerState.LastMove);
		for (const FTankMove& move : UnacknowledgeMoves)
		{
			TankMovementComponent->SimulateMove(move);
		}
	}


	// SpotLight->SetVisibility(ServerState.LastMove.LightOn);
}

void UTankMovementReplicatorComponent::ClearAcknoledgeMoves(FTankMove lastMove)
{
	TArray<FTankMove> newMoves;
	for (const FTankMove& Move : UnacknowledgeMoves)
	{
		if (Move.Time > lastMove.Time)
		{
			newMoves.Add(Move);
		}
	}
	UnacknowledgeMoves = newMoves;
}

void UTankMovementReplicatorComponent::Server_SendMove_Implementation(FTankMove Move)
{
	if (TankMovementComponent != nullptr)
	{
		TankMovementComponent->SimulateMove(Move);

		ServerState.LastMove = Move;
		ServerState.Transform = GetOwner()->GetActorTransform();
		ServerState.Velocity = TankMovementComponent->GetVelocity();
	}
}

bool UTankMovementReplicatorComponent::Server_SendMove_Validate(FTankMove Move)
{
	return true;
}
