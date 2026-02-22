// Fill out your copyright notice in the Description page of Project Settings.


#include "BuzzKillProjectile.h"

#include "GearCharacter.h"
#include "Components/SphereComponent.h"

ABuzzKillProjectile::ABuzzKillProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	// 1. 메쉬를 Root로 확실하게 고정!
	SawMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SawMesh"));
	RootComponent = SawMesh;
    
	// 충돌체는 굳이 안 써도 되지만 쓴다면 메쉬 아래에 붙여줍니다 (SetupAttachment)
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->SetupAttachment(RootComponent);
	// (이후 충돌 이벤트 바인딩은 그대로 둠)
	CollisionComp->OnComponentHit.AddDynamic(this, &ABuzzKillProjectile::OnHit); 
    
	// 외형 및 물리 설정 유지
	SawMesh->SetRelativeScale3D(FVector(0.0007f));
	SawMesh->SetSimulatePhysics(true);
	SawMesh->SetEnableGravity(false);
	SawMesh->SetLinearDamping(0.1f);
    
	// 여기서 SawMesh에 Hit 이벤트를 켤 거면 확실하게 세팅!
	SawMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	SawMesh->SetNotifyRigidBodyCollision(true); // "충돌 시 이벤트를 발생시켜라!"
	SawMesh->BodyInstance.bUseCCD = true; 
    
	SawMesh->BodyInstance.bLockZRotation = true;
	SawMesh->BodyInstance.bLockXRotation = true;
	SawMesh->BodyInstance.bLockYRotation = true;
}

// Called when the game starts or when spawned
void ABuzzKillProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	// Hit 이벤트 바인딩
	SawMesh->OnComponentHit.AddDynamic(this, &ABuzzKillProjectile::OnHit);
	
	// 발사 시 물리적 힘 가하기
	if (SawMesh)
	{
		// [이동] 보는 방향(Forward)으로 강력하게 밀기
		FVector ImpulseDir = GetActorForwardVector();
		SawMesh->AddImpulse(ImpulseDir * LaunchPower, NAME_None, true);

		// [회전] Y축 기준으로 팽이처럼 돌리기
		FVector TorqueDir = FVector(0.0f, 1.0f, 0.0f); 
		SawMesh->AddTorqueInRadians(TorqueDir * SpinPower, NAME_None, true);
	}
	if (GetInstigator()) 
	{
		// 1. 톱날 메쉬가 주인의 캡슐/메쉬를 무시하게 함
		SawMesh->IgnoreActorWhenMoving(GetInstigator(), true);
		
		// 2. 주인도 톱날을 무시하게 함 (양방향 무시)
		GetInstigator()->MoveIgnoreActorAdd(this);
	}
	if (GetOwner())
	{
		SawMesh->IgnoreActorWhenMoving(GetOwner(), true);
	}
}

void ABuzzKillProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// 매 프레임 아래쪽으로 '약한 중력'을 가해줍니다.
	if (SawMesh && SawMesh->IsSimulatingPhysics())
	{
		// 공식: F = m * a (힘 = 질량 * 가속도)
		// 가속도 = 월드 중력(-980) * 우리가 원하는 배율(0.2)
		float GravityZ = GetWorld()->GetGravityZ();
		FVector CustomGravityForce = FVector(0.0f, 0.0f, GravityZ) * CustomGravityScale;
		
		// 액터의 질량을 곱해서 힘(Force)으로 변환해 적용
		SawMesh->AddForce(CustomGravityForce * SawMesh->GetMass());
		
		LastFrameVelocity = SawMesh->GetPhysicsLinearVelocity();
	}
	
}

/*void ABuzzKillProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

	 // 벽/천장 반사 (수동 계산)
	
	// 현재 입사 속도 (V) 가져오기
	FVector IncomingVelocity = SawMesh->GetPhysicsLinearVelocity();

	// 충돌 면의 법선 (N) 가져오기
	FVector Normal = Hit.ImpactNormal;

	// 반사 벡터 공식: R = V - 2(V dot N)N
	// (내적을 통해 벽에 수직인 성분을 구하고, 그 2배만큼 반대 방향으로 뺍니다)
	float DotProduct = FVector::DotProduct(IncomingVelocity, Normal);
	FVector ReflectedVelocity = IncomingVelocity - (2 * DotProduct * Normal);

	// 반발 계수 적용 (속도 조절)
	ReflectedVelocity *= Bounciness;

	// 물리 엔진에 강제 적용
	// (물리 엔진의 기본 마찰력 계산을 무시하고 우리가 계산한 이상적인 반사각을 덮어씌웁니다)
	SawMesh->SetPhysicsLinearVelocity(ReflectedVelocity);
	
	// 바닥 감지 및 파괴
	// 충돌한 면의 법선(Normal)의 Z값이 0.7 이상이면 평평한 바닥으로 간주합니다.
	// (Normal.Z가 1이면 완전 평지, 0이면 수직 벽, -1이면 천장)
	if (Hit.ImpactNormal.Z > 0.7f)
	{
		// 여기에 스파크나 먼지 이펙트를 스폰
		// UGameplayStatics::SpawnEmitterAtLocation(...);

		Destroy(); // 바닥이므로 사라짐
		return;    // 함수 종료
	}
	
	// [수정] 2. 반사각 계산 (직접 물리 구현)
    
	// 현재 속도(GetPhysicsLinearVelocity) 대신 '저장한 속도(LastFrameVelocity)' 사용
	FVector IncomingVelocity = LastFrameVelocity; 

	FVector Normal = Hit.ImpactNormal;

	// 공식: R = V - 2(V dot N)N
	float DotProduct = FVector::DotProduct(IncomingVelocity, Normal);
	FVector ReflectedVelocity = IncomingVelocity - (2 * DotProduct * Normal);

	// 반발 계수 적용
	ReflectedVelocity *= Bounciness;

	// 3. 강제로 새 속도 할당
	SawMesh->SetPhysicsLinearVelocity(ReflectedVelocity);
    
	// (Clipping 방지)
	// 가끔 톱날이 벽 안에 파묻혀서 못 나오는 걸 방지하기 위해 
	// 반사 방향으로 아주 조금(1cm) 이동시킴.
	FVector Nudge = Normal * 1.0f; 
	SetActorLocation(GetActorLocation() + Nudge);
	
	// 디버그용: 튕기는 궤적 그리기
	DrawDebugLine(GetWorld(), Hit.Location, Hit.Location + ReflectedVelocity, FColor::Red, false, 1.0f, 0, 2.0f);
}*/


void ABuzzKillProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    // 1. 유효성 검사 (자기 자신 충돌 방지)
    if (!OtherActor || OtherActor == this) return;

    // 캐릭터 피아 식별 및 명중 판정
    AGearCharacter* HitCharacter = Cast<AGearCharacter>(OtherActor);
    if (HitCharacter)
    {
        AGearCharacter* Shooter = Cast<AGearCharacter>(GetInstigator());
        
        // 아군이거나 쏜 사람 본인이면 무시 (통과하거나 반사하지 않음)
        if (Shooter && (Shooter == HitCharacter || !Shooter->IsHostile(HitCharacter)))
        {
            return; 
        }

        // 적군 명중! (데미지 처리 후 파괴)
        UE_LOG(LogTemp, Warning, TEXT("🎯 [BuzzKill] 적 명중! 톱날 파괴!"));
        
        // TODO: 여기에 HitCharacter의 체력을 깎는 DamageSystem 호출 코드 추가 예정
        
        Destroy(); // 적을 맞췄으니 여기서 파괴하고 함수 종료! (아래 반사 로직 안 탐)
        return; 
    }

	
    // 캐릭터가 아닌 환경(바닥, 벽, 천장)에 부딪혔을 때의 기존 물리 로직
	
    // 바닥 감지 및 파괴
    // 충돌한 면의 법선(Normal)의 Z값이 0.7 이상이면 평평한 바닥으로 간주합니다.
    if (Hit.ImpactNormal.Z > 0.7f)
    {
       // 여기에 스파크나 먼지 이펙트를 스폰
       // UGameplayStatics::SpawnEmitterAtLocation(...);

       Destroy(); // 바닥이므로 사라짐
       return;    // 함수 종료
    }

    // 반사각 계산 (직접 물리 구현)
    // 현재 속도(GetPhysicsLinearVelocity) 대신 '저장한 속도(LastFrameVelocity)' 사용
    FVector IncomingVelocity = LastFrameVelocity; 
    FVector Normal = Hit.ImpactNormal;

    // 공식: R = V - 2(V dot N)N
    float DotProduct = FVector::DotProduct(IncomingVelocity, Normal);
    FVector ReflectedVelocity = IncomingVelocity - (2 * DotProduct * Normal);

    // 반발 계수 적용
    ReflectedVelocity *= Bounciness;

    // 3. 강제로 새 속도 할당
    if (SawMesh) // 안전하게 Mesh가 있는지 한 번 체크
    {
        SawMesh->SetPhysicsLinearVelocity(ReflectedVelocity);
    }
    
    // (Clipping 방지)
    // 가끔 톱날이 벽 안에 파묻혀서 못 나오는 걸 방지하기 위해 
    // 반사 방향으로 아주 조금(1cm) 이동시킴.
    FVector Nudge = Normal * 1.0f; 
    SetActorLocation(GetActorLocation() + Nudge);
    
    // 디버그용: 튕기는 궤적 그리기
    DrawDebugLine(GetWorld(), Hit.Location, Hit.Location + ReflectedVelocity, FColor::Red, false, 1.0f, 0, 2.0f);
}
