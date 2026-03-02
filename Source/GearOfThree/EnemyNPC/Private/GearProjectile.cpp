#include "GearProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"

AGearProjectile::AGearProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

    // 1. 충돌체 세팅
    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    CollisionComp->InitSphereRadius(5.0f);
    CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
    // 부모 클래스에서 미리 OnHit을 연결해둡니다!
    CollisionComp->OnComponentHit.AddDynamic(this, &AGearProjectile::OnHit);
    RootComponent = CollisionComp;

    // 2. 메쉬 세팅
    ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
    ProjectileMesh->SetupAttachment(CollisionComp);

    // 3. 이동 컴포넌트 세팅
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
    ProjectileMovement->UpdatedComponent = CollisionComp;
    ProjectileMovement->InitialSpeed = 3000.f;
    ProjectileMovement->MaxSpeed = 3000.f;
    ProjectileMovement->bRotationFollowsVelocity = true;

    // 4. 나이아가라 궤적 세팅
    TrailEffectComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailEffectComp"));
    TrailEffectComp->SetupAttachment(CollisionComp);
}

void AGearProjectile::BeginPlay()
{
    Super::BeginPlay();
}

void AGearProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    // 부모는 아무것도 안 합니다. 실제 데미지나 이펙트는 자식들이 각자 OnHit을 Override해서 구현합니다!
}

void AGearProjectile::ActivateProjectile(FVector SpawnLocation, FRotator SpawnRotation)
{
    bIsActive = true;
    SetActorLocationAndRotation(SpawnLocation, SpawnRotation);
    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);

    if (ProjectileMovement)
    {
        ProjectileMovement->SetUpdatedComponent(RootComponent);
        ProjectileMovement->Velocity = SpawnRotation.Vector() * ProjectileMovement->InitialSpeed;
        ProjectileMovement->Activate();
    }
}

void AGearProjectile::DeactivateProjectile()
{
    bIsActive = false;
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);

    if (ProjectileMovement)
    {
        ProjectileMovement->StopMovementImmediately();
        ProjectileMovement->Deactivate();
    }
}