// Fill out your copyright notice in the Description page of Project Settings.


#include "Tank.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInput/Public/EnhancedInputComponent.h"
#include "InputConfigData.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "./Tank components/TankMovementComponent.h"


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
	TankMovementReplicatorComponent = CreateDefaultSubobject<UTankMovementReplicatorComponent>(
		TEXT("Tank movement replicator"));
}

// Called when the game starts or when spawned
void ATank::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		NetUpdateFrequency = 1;
	}
	SetReplicateMovement(false);
}


// Called every frame
void ATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


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
	PlayerEnhancedInput->BindAction(InputConfigData->RotateTurret, ETriggerEvent::Triggered, this,
	                                &ATank::RotateTurret);
	PlayerEnhancedInput->BindAction(InputConfigData->RotateTurret, ETriggerEvent::Completed, this,
	                                &ATank::RotateTurret);
	// PlayerEnhancedInput->BindAction(InputConfigData->Light, ETriggerEvent::Completed, this, &ATank::Light);
}

// void ATank::Light()
// {
// 	SpotLight->ToggleVisibility();
// }

UStaticMeshComponent* ATank::GetTurret() const
{
	return Turret;
}

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

void ATank::RotateTurret(const FInputActionValue& Value)
{
	if (TankMovementComponent != nullptr)
	{
		TankMovementComponent->SetTurretRotationValue(Value.Get<float>());
		//Server_Rotate(Value.Get<float>());
	}
}

// //SERVIDOR
//
// void ATank::Server_SendMove_Implementation(FTankMove Move)
// {
// 	
// }
//
// bool ATank::Server_SendMove_Validate(FTankMove Move)
// {
// }
//
// void ATank::OnRep_ServerState()
// {
// 	
// }
//
// void ATank::ClearAcknoledgeMoves(FTankMove lastMove)
// {
// 	
// }

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
