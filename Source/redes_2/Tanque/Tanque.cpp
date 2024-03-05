// Fill out your copyright notice in the Description page of Project Settings.


#include "Tanque.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"


// Sets default values
ATanque::ATanque()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	collider = CreateDefaultSubobject<UBoxComponent>(TEXT("collider"));
	SetRootComponent(collider);

	bodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("bodyMesh"));
	bodyMesh->SetupAttachment(collider);

	turretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("turretMesh"));
	turretMesh->SetupAttachment(bodyMesh);

	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	springArm->SetupAttachment(turretMesh);

	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Cameraa"));
	camera->SetupAttachment(springArm);
}

// Called when the game starts or when spawned
void ATanque::BeginPlay()
{
	Super::BeginPlay();

	//initialize tank values
	position = GetActorLocation();
	velocity = FVector(0);
}

// Called every frame
void ATanque::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//calculate force based on the input(Throttle)
	FVector force = GetActorForwardVector() * maxDrivingForce * Throttle;
	//add air resistance
	force += GetAirResistance();
	//add rolling coeficient
	force += GetRollingResistance();
	//calculate acceleration
	FVector acceleration = (2 * force * DeltaTime) / mass;
	//Update Velocity
	velocity += acceleration;
	//Update position
	position += velocity * 100 * DeltaTime;
	SetActorLocation(position);

	//rotate tank 
	float rotationAngle = maxDegreesPerSecond * DeltaTime * RotationValue;
	FQuat rotationDelta(GetActorUpVector(), FMath::DegreesToRadians(rotationAngle));
	velocity = rotationDelta.RotateVector(velocity);
	AddActorWorldRotation(rotationDelta);
}

// Called to bind functionality to input
void ATanque::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//get player controller
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	//get the Input sub system for inputs from player
	UEnhancedInputLocalPlayerSubsystem* subSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		PlayerController->GetLocalPlayer());

	//clear all current input maps
	subSystem->ClearAllMappings();

	//add our Input map anf get the Enhanced input  component 
	subSystem->AddMappingContext(inputMappingContext, 0);
	UEnhancedInputComponent* playerEnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	//bind Input actions
	//move forward
	playerEnhancedInput->BindAction(InputConfigData->MoveForward, ETriggerEvent::Triggered, this,
	                                &ATanque::MoveForward);
	playerEnhancedInput->BindAction(InputConfigData->MoveForward, ETriggerEvent::Completed, this,
	                                &ATanque::MoveForward);

	//rotate
	playerEnhancedInput->BindAction(InputConfigData->Rotate, ETriggerEvent::Triggered, this,
	                                &ATanque::Rotate);
	playerEnhancedInput->BindAction(InputConfigData->Rotate, ETriggerEvent::Completed, this,
	                                &ATanque::Rotate);
}

void ATanque::MoveForward(const FInputActionValue& axis)
{
	Throttle = axis.Get<float>();
}

void ATanque::Rotate(const FInputActionValue& value)
{
	RotationValue = value.Get<float>();
}

FVector ATanque::GetAirResistance() const
{
	return -velocity.GetSafeNormal() * velocity.Size() * DragCoefficient;
}

FVector ATanque::GetRollingResistance() const
{
	if (velocity.Size() > 0.1f)
	{
		// float AccelerationForGravity = GetWorld()->GetGravityZ() / 100;
		// float normalForce = mass * AccelerationForGravity;
		return -velocity.GetSafeNormal() * RollingResistanceCoefficient;
	}
	return FVector(0);
}
