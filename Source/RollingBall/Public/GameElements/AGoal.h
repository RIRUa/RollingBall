// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AGoal.generated.h"

class UStaticMeshComponent;
class USphereComponent;

UCLASS()
class ROLLINGBALL_API AAGoal : public AActor
{
	GENERATED_BODY()

private:
	/** Goal用のStaticMesh : Goal */
	UPROPERTY(VisibleAnywhere, Category = Goal, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Goal;

	/** 接触判定用のCollision : Sphere */
	UPROPERTY(VisibleAnywhere, Category = Goal, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> Sphere;
	
public:	
	// Sets default values for this actor's properties
	AAGoal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

// Collisionの処理を実装する
private:
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	/** 遷移するLevelのSoft参照 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	TSoftObjectPtr<UWorld> LoadLevel;
};
