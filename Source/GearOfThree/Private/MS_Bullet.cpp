// Fill out your copyright notice in the Description page of Project Settings.


#include "MS_Bullet.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AMS_Bullet::AMS_Bullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 충돌체 등록하기
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	// 충돌 프로파일 설정
	CollisionComponent->SetCollisionProfileName(TEXT("BlockAll"));
	// 충돌체 크기 설정
	CollisionComponent->SetSphereRadius(13); // 미소: 이건 한번 확인해야할듯
	// 루트로 등록
	RootComponent = CollisionComponent;
	
	// 외관 컴포넌트 등록하기
	BodyMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMeshComponent"));
	// 부모 컴포넌트 지정
	BodyMeshComponent->SetupAttachment(CollisionComponent);
	// 충돌 비활성화 // 캐릭터 body 하고는 충돌 x
	BodyMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// 외관 크기 설정 
	BodyMeshComponent->SetRelativeScale3D(FVector(0.25f));
	
	// 발사체의 이동을 담당할 컴포넌트 
	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
	// movement 컴포넌트가 갱신시킬 컴포넌트 지정
	MovementComponent->SetUpdatedComponent(CollisionComponent);
	
	// 여기서 물리 계산이 들어가야 하지 않을까 생각한다. 
	
	// 초기 속도 
	MovementComponent->InitialSpeed = 5000;
	// 최대 속도 
	MovementComponent->MaxSpeed = 5000;
	// 반동 여부 
	MovementComponent->bShouldBounce = true;
	// 반동 값
	MovementComponent->Bounciness = 0.3f;
}

// Called when the game starts or when spawned
void AMS_Bullet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMS_Bullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

