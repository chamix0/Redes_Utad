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

	if (MovementComponent == nullptr) return;

	FTankMove LastMove = MovementComponent->GetLastMove();

	if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
		//MOVIDO A Movement component
		/*FTankMove Move = MovementComponent->CreateMove(DeltaTime);
		MovementComponent->SimulateMove(Move);*/

		UnacknowledgeMoves.Add(LastMove);
		Server_SendMove(LastMove);
	}

	auto Owner = Cast<APawn>(GetOwner());

	if (GetOwnerRole() == ROLE_Authority && Owner->IsLocallyControlled())
	{
		/*FTankMove Move = MovementComponent->CreateMove(DeltaTime);
		Server_SendMove(Move);*/

		UpdateServerState(LastMove);
	}

	if (GetOwnerRole() == ROLE_SimulatedProxy)
	{
		//MovementComponent->SimulateMove(ServerState.LastMove);

		ClientTick(DeltaTime);
	}
}

void UTankMovementReplicatorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTankMovementReplicatorComponent, ServerState)
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

void UTankMovementReplicatorComponent::AutonomousProxy_OnRep_ServerState()
{
	if (MovementComponent == nullptr) return;

	GetOwner()->SetActorTransform(ServerState.Transform);
	MovementComponent->SetVelocity(ServerState.Velocity);

	ClearAcknoledgeMoves(ServerState.LastMove);

	for (const FTankMove& Move : UnacknowledgeMoves)
	{
		MovementComponent->SimulateMove(Move);
	}
}

void UTankMovementReplicatorComponent::SimulatedProxy_OnRep_ServerState()
{
	if (MovementComponent == nullptr) return;

	ClientTimeBetweenLastUpdates = ClientTimeSinceUpdate;
	ClientTimeSinceUpdate = 0;

	ClientStartTransform = GetOwner()->GetActorTransform();

	ClientStartVelocity = MovementComponent->GetVelocity();
}

void UTankMovementReplicatorComponent::UpdateServerState(const FTankMove& Move)
{
	ServerState.LastMove = Move;
	ServerState.Transform = GetOwner()->GetActorTransform();
	ServerState.Velocity = MovementComponent->GetVelocity();
}

void UTankMovementReplicatorComponent::ClientTick(float DeltaTime)
{
	ClientTimeSinceUpdate += DeltaTime;

	if (ClientTimeBetweenLastUpdates < KINDA_SMALL_NUMBER) return;
	if (MovementComponent == nullptr) return;

	//LERP MOVIMIENTO
	FVector TargetLocation = ServerState.Transform.GetLocation();
	FVector StartLocation = ClientStartTransform.GetLocation();
	float LerpRatio = ClientTimeSinceUpdate / ClientTimeBetweenLastUpdates;

	//FVector NewLocation = FMath::LerpStable(StartLocation, TargetLocation, LerpRatio);

	float VelocityToDerivative = ClientTimeBetweenLastUpdates * 100;
	FVector StartDerivative = ClientStartVelocity * VelocityToDerivative;
	FVector TargetDerivative = ServerState.Velocity * VelocityToDerivative;

	FVector NewLocation = FMath::CubicInterp(StartLocation, StartDerivative, TargetLocation, TargetDerivative, LerpRatio);

	GetOwner()->SetActorLocation(NewLocation);

	FVector NewDerivative = FMath::CubicInterpDerivative(StartLocation, StartDerivative, TargetLocation, TargetDerivative, LerpRatio);
	FVector NewVelocity = NewDerivative / VelocityToDerivative;
	MovementComponent->SetVelocity(NewVelocity);

	//LERP ROTATION
	FQuat TargetRotation = ServerState.Transform.GetRotation();
	FQuat StartRotation = ClientStartTransform.GetRotation();
	FQuat NewRotation = FQuat::Slerp(StartRotation, TargetRotation, LerpRatio);

	GetOwner()->SetActorRotation(NewRotation);
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
	if (MovementComponent == nullptr) return;

	ClientSimulatedTime += Move.DeltaTime;

	MovementComponent->SimulateMove(Move);

	//NEW
	/*ServerState.LastMove = Move;
	ServerState.Transform = GetOwner()->GetActorTransform();
	ServerState.Velocity = MovementComponent->GetVelocity();*/

	UpdateServerState(Move);
}

bool UTankMovementReplicatorComponent::Server_SendMove_Validate(FTankMove Move)
{
	float Time = ClientSimulatedTime + Move.DeltaTime;
	bool ClientNormalSpeed = Time <= GetWorld()->TimeSeconds;

	if (!ClientNormalSpeed)
	{
		UE_LOG(LogTemp, Error, TEXT("Client is running too fast!"));
		return false;
	}

	if (!Move.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Received invalid move!"));
		return false;
	}

	return true;
}
