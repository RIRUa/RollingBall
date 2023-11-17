// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/BallPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ArrowComponent.h"

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

	// CollisionPresetを「PhysicsActor」に変更する
	this->Sphere->SetCollisionProfileName(TEXT("PhysicsActor"));

	// Hit Eventを有効にする
	this->Sphere->BodyInstance.bNotifyRigidBodyCollision = true;

	// SpringArmを追加する
	this->SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	this->SpringArm->SetupAttachment(this->RootComponent);

	// Spring Armの長さを調整する
	this->SpringArm->TargetArmLength = 450.0f;

	// PawnのControllerRotationを使用する
	SpringArm->bUsePawnControlRotation = true;

	// CameraのLagを有効にする
	this->SpringArm->bEnableCameraLag = true;
	
	// Cameraを追加する
	this->Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	this->Camera->SetupAttachment(SpringArm);

	this->Camera->PostProcessSettings.MotionBlurAmount = 0.0f;

	// Arrowを追加する
	this->Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	this->Arrow->SetupAttachment(Camera);

	// Sphereの頭上に移動するようにLocationを設定する
	this->Arrow->SetRelativeLocation(FVector(400.0f, 0.0f, 130.0f));
	// Arrowを表示されるようにする
	this->Arrow->bHiddenInGame = false;

	// Input Mapping Context「IM_Controls」を読み込む
	this->DefaultMappingContext = LoadObject<UInputMappingContext>(nullptr, TEXT("/Game/RollingBall/Input/IM_Controls"));

	// Input Action「IA_Control」を読み込む
	this->ControlAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/RollingBall/Input/Action/IA_Control"));

	// Input Action「IA_Look」を読み込む
	this->LookAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/RollingBall/Input/Action/IA_Look"));

	// Input Action「IA_Jump」を読み込む
	this->JumpAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/RollingBall/Input/Action/IA_Jump"));
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

		// LookとIA_LookのTriggeredをBindする
		EnhancedInputComponent->BindAction(this->LookAction, ETriggerEvent::Triggered, this, &ABallPlayer::Look);

		// JumpとIA_JumpのTriggeredをBindする
		EnhancedInputComponent->BindAction(this->JumpAction, ETriggerEvent::Triggered, this, &ABallPlayer::Jump);
	}
}

void ABallPlayer::ControlBall(const FInputActionValue & Value)
{
	const FVector2D V = Value.Get<FVector2D>();

	FVector ForceVector = FVector(V.Y, V.X, 0.0f) * this->Speed;

	this->Sphere->AddForce(ForceVector, NAME_None, true);

	// Arrowの進行方向のVectorを計算する
	FVector ArrowForceVector = this->Arrow->GetComponentToWorld().TransformVectorNoScale(ForceVector);
}

void ABallPlayer::Look(const FInputActionValue& Value)
{
	// inputのValueはVector2Dに変換できる
	FVector2D v = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		this->AddControllerYawInput(v.X);
		this->AddControllerPitchInput(v.Y);

		// Pawnが持っているControlの角度を取得する
		FRotator controlRotate = GetControlRotation();

		// controllerのPitchの角度を制限する
		double LimitPitchAngle = FMath::ClampAngle(controlRotate.Pitch, -40.0f, -10.0f);

		// PlayerControllerの角度を設定する
		UGameplayStatics::GetPlayerController(this, 0)->SetControlRotation(FRotator(controlRotate.Pitch, controlRotate.Yaw, 0.0f));
	}
}

void ABallPlayer::Jump(const FInputActionValue & Value)
{
	// inputのValueはboolに変換できる
	if (const bool V = Value.Get<bool>() && this->CanJump)
	{
		this->Sphere->AddImpulse(FVector(0.0f,0.0f,this->JumpImpulse), TEXT("None"), true);
		CanJump = false;
	}
}

void ABallPlayer::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);	
	this->CanJump = true;
}
