// Fill out your copyright notice in the Description page of Project Settings.


#include "Tank.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInput/Public/EnhancedInputComponent.h"
#include "InputConfigData.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "TankMovementComponent.h"
#include "Components/SpotLightComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ATank::ATank()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;


	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	RootComponent = BoxCollider;

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Body->SetupAttachment(BoxCollider);

	Turret = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret"));
	Turret->SetupAttachment(Body);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(Turret);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	// SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Light"));
	// SpotLight->SetupAttachment(Body);

	TankMovementComponent = CreateDefaultSubobject<UTankMovementComponent>(TEXT("Tank movement component"));
}

// Called when the game starts or when spawned
void ATank::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		NetUpdateFrequency = 1;
	}
}


// Called every frame
void ATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (TankMovementComponent != nullptr)
		{
			FTankMove Move = TankMovementComponent->CreateMove(DeltaTime);
			TankMovementComponent->SimulateMove(Move);
			Server_SendMove(Move);
		}
	}

	if (GetLocalRole() == ROLE_Authority && IsLocallyControlled())
	{
		if (TankMovementComponent != nullptr)
		{
			FTankMove Move = TankMovementComponent->CreateMove(DeltaTime);
			Server_SendMove(Move);
		}
	}

	if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		if (TankMovementComponent != nullptr)
		{
			TankMovementComponent->SimulateMove(ServerState.LastMove);
		}
	}


	//DEBUG
	//DrawDebugString(GetWorld(), FVector(0, 0, -100), GetEnumText(GetLocalRole()), this, FColor::White, DeltaTime);

	FString RoleName;
	UEnum::GetValueAsString(GetLocalRole(), RoleName);
	DrawDebugString(GetWorld(), FVector(0, 0, -100), RoleName, this, FColor::White, DeltaTime);

	//Property Replication

	/*if (HasAuthority())
	{
		ReplicatedTransform = GetActorTransform();
	}*/
}

void ATank::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATank, ServerState)
	/*DOREPLIFETIME(ATank, ReplicatedTransform);
	DOREPLIFETIME(ATank, Throttle);
	DOREPLIFETIME(ATank, RotationValue);
	DOREPLIFETIME(ATank, Velocity);*/
}

// Called to bind functionality to input
void ATank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		PlayerController->GetLocalPlayer());

	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMapping, 0);

	UEnhancedInputComponent* PlayerEnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	PlayerEnhancedInput->BindAction(InputConfigData->MoveForward, ETriggerEvent::Triggered, this, &ATank::MoveForward);
	PlayerEnhancedInput->BindAction(InputConfigData->MoveForward, ETriggerEvent::Completed, this, &ATank::MoveForward);
	PlayerEnhancedInput->BindAction(InputConfigData->Rotate, ETriggerEvent::Triggered, this, &ATank::Rotate);
	PlayerEnhancedInput->BindAction(InputConfigData->Rotate, ETriggerEvent::Completed, this, &ATank::Rotate);

	// PlayerEnhancedInput->BindAction(InputConfigData->Light, ETriggerEvent::Completed, this, &ATank::Light);
}

// void ATank::Light()
// {
// 	SpotLight->ToggleVisibility();
// }

//CLIENTE LOCAL
void ATank::MoveForward(const FInputActionValue& Value)
{
	if (TankMovementComponent != nullptr)
	{
		TankMovementComponent->SetThrottle(Value.Get<float>());
		//Server_MoveForward(Value.Get<float>());
	}
}

void ATank::Rotate(const FInputActionValue& Value)
{
	if (TankMovementComponent != nullptr)
	{
		TankMovementComponent->SetRotationValue(Value.Get<float>());
		//Server_Rotate(Value.Get<float>());
	}
}

//SERVIDOR

void ATank::Server_SendMove_Implementation(FTankMove Move)
{
	if (TankMovementComponent != nullptr)
	{
		TankMovementComponent->SimulateMove(Move);

		ServerState.LastMove = Move;
		ServerState.Transform = GetActorTransform();
		ServerState.Velocity = TankMovementComponent->GetVelocity();
	}
}

bool ATank::Server_SendMove_Validate(FTankMove Move)
{
	return true;
}

void ATank::OnRep_ServerState()
{
	if (TankMovementComponent != nullptr)
	{
		SetActorTransform(ServerState.Transform);
		TankMovementComponent->SetVelocity(ServerState.Velocity);

		ClearAcknoledgeMoves(ServerState.LastMove);
		for (const FTankMove& move : UnacknowledgeMoves)
		{
			TankMovementComponent->SimulateMove(move);
		}
	}


	// SpotLight->SetVisibility(ServerState.LastMove.LightOn);
}

void ATank::ClearAcknoledgeMoves(FTankMove lastMove)
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

//void ATank::Server_MoveForward_Implementation(float value)
//{
//	Throttle = value;
//}
//
//bool ATank::Server_MoveForward_Validate(float value)
//{
//	return FMath::Abs(value) <= 1;
//}

//void ATank::Server_Rotate_Implementation(float value)
//{
//	RotationValue = value;
//}
//
//bool ATank::Server_Rotate_Validate(float value)
//{
//	return FMath::Abs(value) <= 1;
//}


//void ATank::OnRep_ReplicatedTransform()
//{
//	SetActorTransform(ReplicatedTransform);
//}


//With turning radius
//void ATank::ApplyRotation(float DeltaTime)
//{
//	float DeltaLocation = FVector::DotProduct(GetActorForwardVector(), Velocity) * DeltaTime;
//	float RotationAngle = DeltaLocation / MinTurningRadius * RotationValue;
//
//	FQuat RotationDelta(GetActorUpVector(), RotationAngle);
//	Velocity = RotationDelta.RotateVector(Velocity);
//	AddActorWorldRotation(RotationDelta);
//}
