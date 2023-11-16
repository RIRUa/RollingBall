// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "BallPlayer.generated.h"

// Classを前方宣言(includeしないために代替初期化宣言をする)
class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;

UCLASS()
class ROLLINGBALL_API ABallPlayer : public APawn
{
	GENERATED_BODY()

private:
	/** Character用のStaticMesh : Sphere */
	UPROPERTY(VisibleAnywhere, Category = Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Sphere;

	/** Cameraを配置するためのSpringArm */
	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm;

	/** SpringArmの先端に配置するカメラ */
	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;

public:
	// Sets default values for this pawn's properties
	ABallPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

/**ここから移動に関する実装**/
private:
	// 速度
	float Speed = 300.0f;

	// 体力
	float Health = 100.0f;

protected:
	/** BallをControlする */
	void ControlBall(const FInputActionValue& Value);

private:
	/** MappingContext */
	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Control Input Action */
	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ControlAction;
};
