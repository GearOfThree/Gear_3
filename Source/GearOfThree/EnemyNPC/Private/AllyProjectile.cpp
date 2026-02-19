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
	CollisionComp->IgnoreActorWhenMoving(GetOwner(), true);
	
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

void AAllyProjectile::BeginPlay()
{
	Super::BeginPlay();

	// [확인 1] 총알이 세상에 태어났는지 확인
	FString OwnerName = GetOwner() ? GetOwner()->GetName() : TEXT("No Owner");
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, 
		FString::Printf(TEXT("🔫 [Bullet] Spawned! Owner: %s"), *OwnerName));

	// [확인 2] 총알 자체의 콜리전이 주인을 무시하도록 확실하게 설정
	if (GetOwner())
	{
		CollisionComp->IgnoreActorWhenMoving(GetOwner(), true);
	}
}

void AAllyProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 나 자신이나 주인과는 충돌하지 않음
	if (OtherActor && (OtherActor != this) && (OtherActor != GetOwner()))
	{
		// [확인 3] 무엇에 부딪혔는지 로그 출력
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, 
			FString::Printf(TEXT("💥 [Bullet] Hit Actor: %s"), *OtherActor->GetName()));
        
		UE_LOG(LogTemp, Warning, TEXT("Bullet Hit: %s"), *OtherActor->GetName());

		// [확인 4] 부딪힌 위치에 빨간 점 찍기 (3초간 유지)
		DrawDebugSphere(GetWorld(), Hit.Location, 10.0f, 12, FColor::Red, false, 3.0f);

		Destroy();
	}
}
