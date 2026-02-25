#include "AllyProjectile.h"

#include "GearCharacter.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AAllyProjectile::AAllyProjectile()
{
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
    
	// 🚨 [추가된 핵심 코드] 물리 충돌 확실하게 켜기!
	CollisionComp->SetCollisionProfileName(TEXT("BlockAllDynamic")); // 모든 동적 물체와 부딪힘
	CollisionComp->SetNotifyRigidBodyCollision(true); // "부딪히면 OnHit을 반드시 호출해라!"
    
	CollisionComp->OnComponentHit.AddDynamic(this, &AAllyProjectile::OnHit);
	RootComponent = CollisionComp;
	CollisionComp->IgnoreActorWhenMoving(GetOwner(), true);
    
	// 외형 설정
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetRelativeScale3D(FVector(0.5f));
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 메쉬는 충돌 계산 끄기 (최적화)

	// 이동 설정
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	InitialLifeSpan = 3.0f;
	
	// 궤적 이펙트 컴포넌트 생성 및 부착
	TrailEffectComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailEffectComp"));
	TrailEffectComp->SetupAttachment(RootComponent); // 루트(충돌체)를 따라다니게 세팅
}

void AAllyProjectile::BeginPlay()
{
	Super::BeginPlay();

	// 총알 자체의 콜리전이 주인을 무시하도록 설정
	if (GetOwner())
	{
		CollisionComp->IgnoreActorWhenMoving(GetOwner(), true);
	}
}

void AAllyProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == this) return;

	if (AGearCharacter* HitCharacter = Cast<AGearCharacter>(OtherActor)) // 캐릭터(아군 or 적군)를 맞춘 경우
	{
		// 피아 식별 로직 (이전과 동일)
		AGearCharacter* Shooter = Cast<AGearCharacter>(GetInstigator());
		if (Shooter && (Shooter == HitCharacter || !Shooter->IsHostile(HitCharacter)))
		{
			return; // 아군은 관통
		}

		// 맞은 캐릭터의 팀을 확인하고 이펙트 스폰
		if (HitCharacter->GetTeamSide() == ETeamSide::Enemy)
		{
			// 적(사이언) 명중!
			if (EnemyHitEffect) 
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), EnemyHitEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}
		}
	}
	else // 캐릭터가 아닌 벽/바닥을 맞춘 경우
	{
		// 벽 피격 이펙트
		if (WallHitEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), WallHitEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
		}
	}

	Destroy(); 
}
