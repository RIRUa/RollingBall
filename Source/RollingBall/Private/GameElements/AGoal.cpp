// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/AGoal.h"
#include "Character/BallPlayer.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAGoal::AAGoal()
{
 	// StaticMeshComponentを追加し、RootComponentに設定する
	this->Goal = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	this->RootComponent = this->Goal;

	// StaticMeshをLaodしてStaticMeshComponentのStaticMeshに設定する
	UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/RollingBall/Enviroment/Meshes/S_Goal"));

	// StaticMeshをStaticMeshComponentに設定する
	this->Goal->SetStaticMesh(Mesh);

	// SphereCollisionを追加する
	this->Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	this->Sphere->SetupAttachment(this->RootComponent);

	// Radiusを設定する
	this->Sphere->SetSphereRadius(72.0f);

	// AAGoalのOnComponentBeginOverlapをBindする
	this->Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAGoal::OnSphereBeginOverlap);
}

// Called when the game starts or when spawned
void AAGoal::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAGoal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAGoal::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (const ABallPlayer* Player = Cast<ABallPlayer>(OtherActor))
	{
		/**ゴールに辿り着いた際の挙動確認用のログ**/
		//UE_LOG(LogTemp, Display, TEXT("Goal"));

		UGameplayStatics::OpenLevelBySoftObjectPtr(this, this->LoadLevel);
	}
}
