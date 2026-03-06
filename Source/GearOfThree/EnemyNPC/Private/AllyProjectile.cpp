#include "AllyProjectile.h"
#include "GearCharacter.h"
#include "MS_DamageableCharacter.h"
#include "NiagaraComponent.h"
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
          // 1. 피격 이펙트 재생
          if (EnemyHitEffect) 
          {
             UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), EnemyHitEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
          }
          
          // =====================================================================
          // 🚨 [추가된 동료분의 데미지 시스템 연동 로직]
          // =====================================================================
          // GearCharacter 를 상속 받고 있는 대상인지 확인한다.
          AMS_DamageableCharacter* DamageableChar = Cast<AMS_DamageableCharacter>(OtherActor);
          
          // return 대신 if문으로 감싸서 풀링 수면 함수가 정상 작동하게 보호합니다.
          if (DamageableChar)
          {
              // UMS_Damageable 를 구현하고 있는지 확인한다. && TeamSide 가 Enemy 인지 확인한다. 
              // (위에서 Enemy 체크를 했지만, 확실하게 한 번 더 체크!)
              if (OtherActor->Implements<UMS_Damageable>() && DamageableChar->TeamSide == ETeamSide::Enemy)
              {
                  // 1 : 함수를 실행할 대상 객체, 2 : 데미지 수치(Power), 3 : 가해자(this)
                  IMS_Damageable::Execute_ReceiveDamage(OtherActor, Power, this);
                  
                  DrawDebugString(
                     GetWorld(),
                     GetActorLocation() + FVector(0,0,100),
                     TEXT("Ally Damage Applied!"),
                     nullptr,
                     FColor::White,
                     2.0f,
                     true
                  ); 
              }
          }
          // =====================================================================
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

    // 🚨 [핵심 유지] Destroy() 대신 풀링으로 돌려보내기
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
   if (TrailEffectComp)
   {
      // 파라미터로 true를 넘겨주면 "기존 상태를 리셋하고 처음부터 다시 재생하라"는 뜻입니다.
      TrailEffectComp->Activate(true); 
   }
}

// (3초가 지났거나, OnHit에서 호출되었거나)
void AAllyProjectile::DeactivateProjectile()
{
   // 부모의 기본 수면 로직 실행 (숨기기, 물리 이동 정지 등)
   Super::DeactivateProjectile();
   
   GetWorld()->GetTimerManager().ClearTimer(DeactivateTimerHandle);
   
   if (TrailEffectComp)
   {
      // 파라미터로 true를 넘겨주면 "기존 상태를 리셋하고 처음부터 다시 재생하라"는 뜻입니다.
      TrailEffectComp->Deactivate(); 
   }
}