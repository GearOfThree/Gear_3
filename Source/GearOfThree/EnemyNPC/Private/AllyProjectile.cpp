#include "AllyProjectile.h"

#include "GearCharacter.h"
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
	// 1. 자기 자신이나 허공에 부딪힌 오류 방지
	if (!OtherActor || OtherActor == this) return;

	// 2. 누가 이 총알을 쐈는가? (Instigator: 무기 컴포넌트에서 쏠 때 넣어둔 주인 정보)
	AGearCharacter* Shooter = Cast<AGearCharacter>(GetInstigator());
    
	// 3. 맞은 대상이 캐릭터(GearCharacter)인가?
	AGearCharacter* HitCharacter = Cast<AGearCharacter>(OtherActor);

	if (Shooter && HitCharacter)
	{
		// 쏜 사람이 자기가 쏜 총알에 맞거나(달려가면서 쏠 때), 같은 팀(아군)을 맞춘 경우
		if (Shooter == HitCharacter || !Shooter->IsHostile(HitCharacter))
		{
			// 아군 통과 (총알이 안 터지고 그대로 뚫고 지나가게 합니다)
			return; 
		}

		// 적군을 맞췄을 때
		UE_LOG(LogTemp, Warning, TEXT("[Hit] 적 명중 투사체 파괴"));
        
		// (나중에 여기에 DamageSystem을 호출해서 체력을 깎는 코드를 넣습니다)
	}
	else
	{
		// 대상이 캐릭터가 아님 -> 즉, '벽'이나 '바닥' 등 배경에 맞았을 때
		UE_LOG(LogTemp, Log, TEXT("[Hit] 벽/바닥 명중 투사체 파괴"));
	}

	// 아군을 맞춘 게 아니라면 (적군 or 벽에 맞았으므로) 투사체 소멸
	Destroy(); 
}
