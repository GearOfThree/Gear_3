#include "AllyProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AAllyProjectile::AAllyProjectile()
{
	// 충돌체 설정
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->OnComponentHit.AddDynamic(this, &AAllyProjectile::OnHit); // 충돌 이벤트 연결
	RootComponent = CollisionComp;

	// 외형 설정
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetRelativeScale3D(FVector(0.5f)); // 크기 조절

	// 이동 설정
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f; // 총알 속도
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true; // 날아가는 방향으로 회전
	ProjectileMovement->ProjectileGravityScale = 0.0f; // 중력 영향 없음 (직선 발사)

	InitialLifeSpan = 3.0f; // 3초 뒤 자동 소멸
}

void AAllyProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 여기에 나중에 데미지 로직과 파티클 효과를 추가할 겁니다.
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		Destroy(); // 일단 부딪히면 사라지게 설정
	}
}

