#include "AllyProjectile.h"
#include "GearCharacter.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AAllyProjectile::AAllyProjectile()
{
   PrimaryActorTick.bCanEverTick = false;
   
    if (CollisionComp)
    {
       // 물리 충돌 프로필 및 오너 무시 설정
       CollisionComp->SetCollisionProfileName(TEXT("BlockAllDynamic"));
       CollisionComp->SetNotifyRigidBodyCollision(true);
    }

    if (ProjectileMesh)
    {
       // 샐리 총알 메쉬 크기와 충돌 끄기
       ProjectileMesh->SetRelativeScale3D(FVector(0.5f));
       ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    if (ProjectileMovement)
    {
       // 샐리 총알은 중력의 영향을 받지 않고 일직선으로 날아감
       ProjectileMovement->ProjectileGravityScale = 0.0f;
    }
}

void AAllyProjectile::BeginPlay()
{
    Super::BeginPlay();

    // 총알 자체의 콜리전이 주인을 무시하도록 설정
    if (GetOwner() && CollisionComp)
    {
       CollisionComp->IgnoreActorWhenMoving(GetOwner(), true);
    }
}

void AAllyProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!OtherActor || OtherActor == this) return;

    if (AGearCharacter* HitCharacter = Cast<AGearCharacter>(OtherActor)) // 캐릭터를 맞춘 경우
    {
       AGearCharacter* Shooter = Cast<AGearCharacter>(GetInstigator());
       
       // 아군이거나 쏜 사람 본인이면 무시 (관통)
       if (Shooter && (Shooter == HitCharacter || !Shooter->IsHostile(HitCharacter)))
       {
          return; 
       }

       // 적(사이언) 명중!
       if (HitCharacter->GetTeamSide() == ETeamSide::Enemy)
       {
          if (EnemyHitEffect) 
          {
             UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), EnemyHitEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
          }
          
          // TODO: 나중에 여기에 데미지 주는 로직 한 줄 추가 (DamageComp->TakeCustomDamage(...))
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

    // 🚨 [핵심 수정] Destroy()를 지우고, 총알을 재워서 탄약고(Pool)로 돌려보냅니다!
    DeactivateProjectile(); 
}

void AAllyProjectile::ActivateProjectile(FVector SpawnLocation, FRotator SpawnRotation)
{
   // 1. 부모의 기본 발사 로직 실행 (위치 이동, 보이기, 물리 이동 시작 등)
   Super::ActivateProjectile(SpawnLocation, SpawnRotation);

   // "3초 뒤에 DeactivateProjectile 함수를 실행"
   GetWorld()->GetTimerManager().SetTimer(
       DeactivateTimerHandle, 
       this, 
       &AAllyProjectile::DeactivateProjectile, 
       3.0f, 
       false
   );
}

// (3초가 지났거나, OnHit에서 호출되었거나)
void AAllyProjectile::DeactivateProjectile()
{
   // 부모의 기본 수면 로직 실행 (숨기기, 물리 이동 정지 등)
   Super::DeactivateProjectile();
   
   GetWorld()->GetTimerManager().ClearTimer(DeactivateTimerHandle);
}