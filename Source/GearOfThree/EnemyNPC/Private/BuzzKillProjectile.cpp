// Fill out your copyright notice in the Description page of Project Settings.


#include "BuzzKillProjectile.h"

// Sets default values
ABuzzKillProjectile::ABuzzKillProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SawMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SawMesh"));
	RootComponent = SawMesh;
	
	//톱날이 너무 큼;;
	SawMesh->SetRelativeScale3D(FVector(0.0007f));
	//물리 시뮬레이션 활성화
	SawMesh->SetSimulatePhysics(true);
	//중력 적용
	SawMesh->SetEnableGravity(false);
	
	// 공기 저항
	SawMesh->SetLinearDamping(0.1f);
	
	//충돌 프로필 설정
	SawMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	SawMesh->SetNotifyRigidBodyCollision(true);
	
	// 연속 충돌 감지 (CCD) 켜기 - 톱날이 벽을 뚫고 나가는 것을 방지
	SawMesh->BodyInstance.bUseCCD = true; 
	
	// 톱날이 동전처럼 옆으로 구르지 않고 수평을 유지하며 날아가게 회전 축 잠금.
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

void ABuzzKillProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
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

	/*
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
	*/
	
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
}