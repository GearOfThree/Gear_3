// Fill out your copyright notice in the Description page of Project Settings.


#include "Leech/LeechManager.h"

#include "AllyNPCCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ALeechManager::ALeechManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SetRootComponent(SphereComp);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

// Called when the game starts or when spawned
void ALeechManager::BeginPlay()
{
	Super::BeginPlay();
	
	// 현재 스폰 위치 저장
	SpawnLocation = GetActorLocation();
	
	// 시작과 동시에 리치 무리 스폰
	SpawnLeeches();
	
	// 초기 방향 설정
	ChangeDirection();
	
	// 방향 전환 타이머 설정
	GetWorldTimerManager().SetTimer(
	   TimerHandle_ChangeDir,
	   this,
	   &ALeechManager::ChangeDirection,
	   2.0f,
	   true
   );
	
	// 감염 타이머 설정
	GetWorldTimerManager().SetTimer(
	   TimerHandle_Infect,
	   this,
	   &ALeechManager::InfectionDirection,
	   60.0f,
	   false
   );
}

// Called every frame
void ALeechManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// 현재 위치 계산
	FVector NowLocation = GetActorLocation();
	
	// 감염 가능하면
	if (bInfectable)
	{
		// 아군 NPC 검사해서 돌진 - Tick 내부의 for문이 너무 무거우면 다른 방향으로 수정 
		for (AActor* Actor : NPC)
		{
			AGearCharacter* target = Cast<AGearCharacter>(Actor);
			if (!target) continue;
			if (target->TeamSide == ETeamSide::Ally)
			{
				Direction = (target->GetActorLocation() - NowLocation).GetSafeNormal();
				SetActorLocation(NowLocation + Direction * 2000 * DeltaTime);
				return;
			}
		}
		// 검사했는데 그새 아군 NPC가 다 죽어버렸거나 이미 다 감염됐다면
		// 1초 동안 위로 솟구침
		if (OffsetTime >= 0)
		{
			SetActorLocation(NowLocation + FVector(0,0,2000) * DeltaTime);
			OffsetTime -= DeltaTime;
			return; // 1초가 지나야만 다음으로 진행 가능
		}
		// 감염 가능 여부를 다시 false로 변경하고
		bInfectable = false;
		// 모든 리치들의 상태를 Fall로 변경
		for (ALeech* Leech : SpawnedLeeches)
		{
			if (!IsValid(Leech)) continue;
			Leech->OrbitToFall();
		}
		// 안전하게 그 이후 5초가 지나면
		if (OffsetTime >= -5.f)
		{
			OffsetTime -= DeltaTime;
			return; // 5초가 지나야만 다음으로 진행 가능
		}
		Destroy();
	}
	
	// 스폰 지점으로부터의 거리 계산
	double Distance = UKismetMathLibrary::VSize(NowLocation - SpawnLocation);
	
	// 거리가 너무 떨어지거나 높이 값이 튀면
	if (Distance > 2500 || NowLocation.Z < 300 || NowLocation.Z > 800)
	{
		// 방향을 스폰 지점으로 설정
		Direction = (SpawnLocation - NowLocation).GetSafeNormal();
	}
	SetActorLocation(NowLocation + Direction * Speed * DeltaTime);
}

void ALeechManager::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (auto* target = Cast<AAllyNPCCharacter>(OtherActor))
	{
		target->SetTeamSide(true);
	}
}

void ALeechManager::SpawnLeeches()
{
	// 월드 유효성 검사
	if (!IsValid(GetWorld())) return;

	// BluePrint에서 스폰 공장을 직접 지정해주지 않았다면
	if (!LeechFactory)
	{
		// 로그 띄우기, 이후 내용 실행 X
		UE_LOG(LogTemp, Warning, TEXT("ALeechManager: LeechClass is not set."));
		return;
	}

	// SpawnCount가 너무 높으면 적당히 200에서 자르기 (메모리)
	const int32 Count = FMath::Clamp(SpawnCount, 1, 200);
	// 호옥시라도 플록을 여러 개 소환하거나 이전의 잔재가 남아있을 경우를 대비하여 배열을 한 번 싹 비우기
	SpawnedLeeches.Reset();
	// 동적 할당된 배열의 크기를 미리 소환할 리치 객체 수만큼 확보해두기
	SpawnedLeeches.Reserve(Count);

	// 소환할 마릿수 만큼 소환 로직 반복
	for (int32 i = 0; i < Count; ++i)
	{
		// 스폰 시 Transform (위치 값, 회전 값) 저장 변수 생성
			// 이때 매니저 자신의 값으로 세팅
		FTransform SpawnTM(GetActorRotation(), GetActorLocation());

		// 월드에 리치를 스폰하고 스폰한 리치를 Leech에 저장
			// 그냥 스폰하면 모든 리치의 인덱스가 0이 되는 현상 발생(전부 같은 값들을 가지게 됨)
			// 소환된 리치의 BeginPlay 전에 값 주입을 보장(Deferred)
		ALeech* Leech = GetWorld()->SpawnActorDeferred<ALeech>(
			LeechFactory,	// 소환할 클래스
			SpawnTM,		// 소환 Transform
			this,			// Owner(리치 소유자)
			nullptr,		// Instigator (지금 당장 AIController 없으니 nullptr)
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn // 겹치든 말든 그냥 항상 스폰해라
		);

		if (!Leech)
		{
			continue;	// 스폰 실패 시 그 객체만 건너뛰고 진행
		}

		// Deferred로 스폰했기 때문에 아직 리치의 BeginPlay가 실행되지 않음
		// 여기서 값을 세팅하면 각 리치마다 고유의 값이 제대로 주입됨
		Leech->LeechManager = this;							// 리치 소유자는 소환시킨 매니저
		Leech->OrbitIndex = i;								// 너 몇 번째 리치니?
		Leech->OrbitCount = Count;							// 전체 마릿수
		Leech->OrbitRadius = OrbitRadius;					// 공전 반지름
		Leech->OrbitSpeedDegPerSec = OrbitSpeedDegPerSec;	// 공전 각속도
		
		// 리치마다 랜덤한 축을 기준으로 기울기를 줌
		FRandomStream Rng(time(NULL) + i);
			// 1) 랜덤한 방향 벡터(회전축) 생성
		const FVector Axis = Rng.VRand();	// 단위벡터 랜덤
			// 2) 기울기 각도 설정
		const float TiltDeg = Rng.FRandRange(0.f, MaxTiltDeg);	// 기울기 각도 랜덤
			// 3) 그 축으로 기울기 회전(Quat)
		Leech->OrbitPlaneQuat = FQuat(Axis, FMath::DegreesToRadians(TiltDeg));

		// 할당 종료 후 스폰 완료 호출 -> BeginPlay 진행
		UGameplayStatics::FinishSpawningActor(Leech, SpawnTM);
		// 스폰된 리치들 리스트에 객체 추가
		SpawnedLeeches.Add(Leech);
	}
}

void ALeechManager::ChangeDirection()
{
	// 디폴트 : 영벡터(이동 중지)
	Direction = FVector::Zero();
	
	// 이동 가능 상황이면
	if (Moveable)
	{
		// 랜덤한 방향 벡터 설정
		Direction = UKismetMathLibrary::RandomUnitVector();
		// 연직 방향으로 너무 튀지 않게 Z값만 조정
		Direction.Z = FMath::Clamp(Direction.Z, -0.3f, 0.3f);
		// 다음 호출 시 이동 불가능하도록 설정하고 함수 종료
		Moveable = false;
		return;
	}
	
	// 이동 불가능 상황이면
	// 방향은 그대로 영벡터, 다음 호출 시 이동 가능하도록 설정
	Moveable = true;
}

void ALeechManager::InfectionDirection()
{
	// 현재 궤도를 돌고 있는 리치 검사
	int32 IsOrbitingNumber = 0;
	for (ALeech* Leech : SpawnedLeeches)
	{
		if (!IsValid(Leech)) continue;

		if (Leech->bOrbiting)
		{
			IsOrbitingNumber++;
			Leech->OrbitRadius = 100.f;
		}
	}
	
	// 현재 궤도를 돌고 있는 리치가 없으면 감염 불가능
	if (IsOrbitingNumber == 0) return;
	
	// 감염 대상 판단 후 가능할 경우 변수 설정
	UGameplayStatics::GetAllActorsOfClass(this, AGearCharacter::StaticClass(), NPC);
	for (AActor* Actor : NPC)
	{
		AGearCharacter* target = Cast<AGearCharacter>(Actor);
		if (!target) continue;
		if (target->TeamSide == ETeamSide::Ally)
		{
			bInfectable = true;
			return;
		}
	}
	// 다 검사했는데 감염시킬 대상이 없음
	// 모든 리치들의 상태를 Fall로 변경
	for (ALeech* Leech : SpawnedLeeches)
	{
		if (!IsValid(Leech)) continue;
		Leech->OrbitToFall();
	}
}
