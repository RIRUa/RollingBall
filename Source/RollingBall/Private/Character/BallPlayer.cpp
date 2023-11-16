// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/BallPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"

// Sets default values
ABallPlayer::ABallPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// StaticMeshComponentを追加し、RootComponentに設定する
	this->Sphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	this->RootComponent = this->Sphere;

	// StaticMeshをLaodしてStaticMeshComponentのStaticMeshに設定する
	UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere"));
	// StaticMeshをStaticMeshComponentに設定する
	this->Sphere->SetStaticMesh(Mesh);

	// MaterialをStaticMeshに設定する
	UMaterial* Material = LoadObject<UMaterial>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial"));
	// MaterialをStaticMeshComponentに設定する
	this->Sphere->SetMaterial(0, Material);

	// Simulate Physicsを有効にする
	this->Sphere->SetSimulatePhysics(true);

	// SpringArmを追加する
	this->SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	this->SpringArm->SetupAttachment(this->RootComponent);
	
	// 角度を変更する FRotator(Pitch(Y), Yaw(Z), Roll(X))
	this->SpringArm->SetRelativeRotation(FRotator(-30.0f, 0.0f, 0.0f));

	// Spring Armの長さを調整する
	this->SpringArm->TargetArmLength = 450.0f;

	// SpringArmからの角度を継承しない
	this->SpringArm->bInheritPitch = false;
	this->SpringArm->bInheritYaw = false;
	this->SpringArm->bInheritRoll = false;

	// CameraのLagを有効にする
	this->SpringArm->bEnableCameraLag = true;
	
	// Cameraを追加する
	this->Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	this->Camera->SetupAttachment(SpringArm);

	this->Camera->PostProcessSettings.MotionBlurAmount = 0.0f;

	// Input Mapping Context「IM_Controls」を読み込む
	this->DefaultMappingContext = LoadObject<UInputMappingContext>(nullptr, TEXT("/Game/RollingBall/Input/IM_Controls"));

	// Input Action「IA_Control」を読み込む
	this->ControlAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/RollingBall/Input/Action/IA_Control"));
}

// Called when the game starts or when spawned
void ABallPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	if (const APlayerController* PlayerController = Cast<APlayerController>(this->Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

// Called to bind functionality to input
void ABallPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		// ControlBallとIA_ControlのTriggeredをBindする
		EnhancedInputComponent->BindAction(this->ControlAction, ETriggerEvent::Triggered, this, &ABallPlayer::ControlBall);
	}
}

void ABallPlayer::ControlBall(const FInputActionValue & Value)
{
	const FVector2D V = Value.Get<FVector2D>();

	FVector ForceVector = FVector(V.Y, V.X, 0.0f) * this->Speed;

	this->Sphere->AddForce(ForceVector, NAME_None, true);
}
