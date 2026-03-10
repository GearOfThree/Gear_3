#include "BuzzKillProjectile.h"
#include "GearCharacter.h"
#include "MS_DamageableCharacter.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

ABuzzKillProjectile::ABuzzKillProjectile()
{
    PrimaryActorTick.bCanEverTick = true;
    
    // (CreateDefaultSubobject만 생략하고, 부모의 ProjectileMesh를 그대로 활용합니다)
    if (ProjectileMesh)
    {
        ProjectileMesh->SetupAttachment(nullptr);
        SetRootComponent(ProjectileMesh); // 메쉬를 Root로!

        ProjectileMesh->SetRelativeScale3D(FVector(0.0007f));
        ProjectileMesh->SetSimulatePhysics(true);
        ProjectileMesh->SetEnableGravity(false);
        ProjectileMesh->SetLinearDamping(0.1f);

        ProjectileMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
        ProjectileMesh->SetNotifyRigidBodyCollision(true);
        
        ProjectileMesh->OnComponentHit.AddDynamic(this, &ABuzzKillProjectile::OnHit);
        
        ProjectileMesh->BodyInstance.bUseCCD = true;
        ProjectileMesh->BodyInstance.bLockZRotation = true;
        ProjectileMesh->BodyInstance.bLockXRotation = true;
        ProjectileMesh->BodyInstance.bLockYRotation = true;
    }

    if (CollisionComp)
    {
        CollisionComp->InitSphereRadius(5.0f);
        CollisionComp->SetupAttachment(ProjectileMesh);
        CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    if (TrailEffectComp)
    {
        TrailEffectComp->SetupAttachment(ProjectileMesh);
    }
    if (ProjectileMovement)
    {
        // 🚨 부모가 물려준 이동 컴포넌트의 전원을 완전히 뽑아버립니다!
        ProjectileMovement->Deactivate(); 
        ProjectileMovement->SetUpdatedComponent(nullptr); 
    }
}

void ABuzzKillProjectile::BeginPlay()
{
    Super::BeginPlay();
}

// 톱날 깨우기
void ABuzzKillProjectile::ActivateProjectile(FVector SpawnLocation, FRotator SpawnRotation)
{
    // 부모의 ProjectileMovement가 켜지면 물리 엔진과 충돌해서 공중에 멈춰버립니다.
    
    bIsActive = true;
    SetActorHiddenInGame(false); // 눈에 보이게 켜기

    if (ProjectileMesh)
    {
       // 1. 충돌을 끈 상태로 순간이동 먼저! (캐릭터 몸통에 걸리는 것 방지)
       ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
       SetActorLocationAndRotation(SpawnLocation, SpawnRotation, false, nullptr, ETeleportType::TeleportPhysics);

       // 2. 주인(쏜 사람) 무시 확실하게 세팅
       if (GetInstigator()) 
       {
           ProjectileMesh->IgnoreActorWhenMoving(GetInstigator(), true);
           GetInstigator()->MoveIgnoreActorAdd(this);
       }
       if (GetOwner())
       {
           ProjectileMesh->IgnoreActorWhenMoving(GetOwner(), true);
       }

       // 3. 이동이 끝난 후 물리 및 충돌 켜기
       ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
       ProjectileMesh->SetSimulatePhysics(true);
       ProjectileMesh->WakeAllRigidBodies();

       // 4. 이전 속도 초기화
       ProjectileMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
       ProjectileMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

       // 5. 뻥 차기! (이제 방해꾼이 없어서 무조건 날아갑니다)
       FVector ImpulseDir = SpawnRotation.Vector(); 
       ProjectileMesh->AddImpulse(ImpulseDir * LaunchPower, NAME_None, true);

       FVector TorqueDir = FVector(0.0f, 1.0f, 0.0f); 
       ProjectileMesh->AddTorqueInRadians(TorqueDir * SpinPower, NAME_None, true);
    }
    
    if (TrailEffectComp)
    {
        // 파라미터로 true를 넘겨주면 "기존 상태를 리셋하고 처음부터 다시 재생하라"는 뜻입니다.
        TrailEffectComp->Activate(true); 
    }
}

// 💤 톱날 재우기 (풀링 대기)
void ABuzzKillProjectile::DeactivateProjectile()
{
    // 여기도 Super::DeactivateProjectile() 금지
    
    bIsActive = false;
    SetActorHiddenInGame(true); // 눈에 안 보이게 숨기기

    if (ProjectileMesh)
    {
        // 허공에 멈춰있게 하기 위해 속도 0으로 초기화
        ProjectileMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
        ProjectileMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

        // 물리 연산 끄고, 충돌도 완전히 꺼버림
        ProjectileMesh->SetSimulatePhysics(false);
        ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    if (TrailEffectComp)
    {
        // 파라미터로 true를 넘겨주면 "기존 상태를 리셋하고 처음부터 다시 재생하라"는 뜻입니다.
        TrailEffectComp->Deactivate(); 
    }
}

void ABuzzKillProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 💡 풀링 대기 중일 때는 연산하지 않음
    if (!bIsActive) return; 

    // 🚨 유저 원본 커스텀 중력 로직 유지!
    if (ProjectileMesh && ProjectileMesh->IsSimulatingPhysics())
    {
        float GravityZ = GetWorld()->GetGravityZ();
        FVector CustomGravityForce = FVector(0.0f, 0.0f, GravityZ) * CustomGravityScale;
        
        ProjectileMesh->AddForce(CustomGravityForce * ProjectileMesh->GetMass());
        LastFrameVelocity = ProjectileMesh->GetPhysicsLinearVelocity();
    }
}

void ABuzzKillProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    // 1. 유효성 검사 (자기 자신 충돌 방지)
    if (!OtherActor || OtherActor == this) return;
    
    // 2. 캐릭터 피아 식별 및 이펙트 처리
    AGearCharacter* HitCharacter = Cast<AGearCharacter>(OtherActor);
    if (HitCharacter)
    {
        AGearCharacter* Shooter = Cast<AGearCharacter>(GetInstigator());
        
        if (Shooter && (Shooter == HitCharacter || !Shooter->IsHostile(HitCharacter)))
        {
            return;
        }
        
        if (HitCharacter->GetTeamSide() == ETeamSide::Ally || HitCharacter->GetTeamSide() == ETeamSide::Player)
        {
            if (AllyHitEffect)
            {
                UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), AllyHitEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
            }
        }
        
        // GearCharacter 를 상속 받고 있는 대상인지 확인한다.
        AMS_DamageableCharacter* DamageableChar = Cast<AMS_DamageableCharacter>(OtherActor);
        
        if (DamageableChar)
        {
            // UMS_Damageable 를 구현하고 있는지 확인한다. && TeamSide 가 Enemy 인지 확인한다. 
            if (OtherActor->Implements<UMS_Damageable>() && (DamageableChar->TeamSide == ETeamSide::Ally || DamageableChar->TeamSide == ETeamSide::Player))
            {
                // 1: 함수를 실행할 대상 객체, 2: 데미지 수치(Power), 3: 가해자(this)
                IMS_Damageable::Execute_ReceiveDamage(OtherActor, Power, this);
                
                DrawDebugString(
                   GetWorld(),
                   GetActorLocation() + FVector(0,0,100),
                   TEXT("Sion Damage Applied!"), // 빈칸이었던 부분에 확인용 텍스트 임시 추가
                   nullptr,
                   FColor::White,
                   2.0f,
                   true
                ); 
            }
        }
        // =====================================================================

        // 맞췄으니 파괴(풀링 수면) 처리
        DeactivateProjectile(); 
        return;
    }

    // 3. 환경(바닥) 충돌
    if (Hit.ImpactNormal.Z > 0.7f)
    {
        if (BounceSparkEffect)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BounceSparkEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
        }
        DeactivateProjectile(); 
        return;
    }

    // 4. 벽/천장 반사 물리 및 스파크 이펙트
    if (BounceSparkEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BounceSparkEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
    }

    FVector IncomingVelocity = LastFrameVelocity;
    FVector Normal = Hit.ImpactNormal;

    float DotProduct = FVector::DotProduct(IncomingVelocity, Normal);
    FVector ReflectedVelocity = IncomingVelocity - (2 * DotProduct * Normal);
    ReflectedVelocity *= Bounciness;

    if (ProjectileMesh)
    {
        ProjectileMesh->SetPhysicsLinearVelocity(ReflectedVelocity);
    }
    
    FVector Nudge = Normal * 1.0f;
    SetActorLocation(GetActorLocation() + Nudge);
    //DrawDebugLine(GetWorld(), Hit.Location, Hit.Location + ReflectedVelocity, FColor::Red, false, 1.0f, 0, 2.0f);
}