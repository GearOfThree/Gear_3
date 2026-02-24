// Fill out your copyright notice in the Description page of Project Settings.


#include "MS_Bullet.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

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
	CollisionComponent->SetSphereRadius(13);
	// 루트로 등록
	RootComponent = CollisionComponent;
	
	// 외관 컴포넌트 등록하기
	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	BulletMesh->SetupAttachment(CollisionComponent);
	
	
	BulletMesh->SetSimulatePhysics(true); // 충돌처리 하기 위해서 
	BulletMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	// Hit 이벤트를 받기 위한 설정 
	BulletMesh->SetNotifyRigidBodyCollision(true); // 물리 기반의 충돌 이벤트를 받을지 확인한다. 
	BulletMesh->SetGenerateOverlapEvents(false);
	
	// 고속일시 설정
	BulletMesh->BodyInstance.bUseCCD = true;
	
	BulletMesh->SetRelativeScale3D(FVector(0.25f)); // 외관 크기 설정
	
	
	
	// BodyMeshComponent->SetSimulatePhysics(true); // 물리 시뮬레이션 on
	// BodyMeshComponent->SetEnableGravity(true); // 중력 on
	
	// 발사체의 이동을 담당할 컴포넌트 
	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
	// movement 컴포넌트가 갱신시킬 컴포넌트 지정
	MovementComponent->SetUpdatedComponent(CollisionComponent);
	
	 
	
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
	
	BulletMesh->OnComponentHit.AddDynamic(this, &AMS_Bullet::OnBulletHit);
}

// Called every frame
void AMS_Bullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMS_Bullet::OnBulletHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("OnBulletHit! Other=%s Point=%s Normal=%s"),
	*GetNameSafe(OtherActor),
	*Hit.ImpactPoint.ToString(),
	*Hit.ImpactNormal.ToString());

	DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 20.f, 12, FColor::Red, false, 2.f);
	
	// 자기 자신 충돌 방지
	if (!OtherActor || OtherActor == this) return;
	
	if (ImpactFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ImpactFX,
			Hit.ImpactPoint,
			Hit.ImpactNormal.Rotation(),
			true // AutoDestroy
		);
		// 총알 제거 
		Destroy();
	}
	
}

