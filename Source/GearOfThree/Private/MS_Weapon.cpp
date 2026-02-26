// Fill out your copyright notice in the Description page of Project Settings.


#include "MS_Weapon.h"
#include "NiagaraFunctionLibrary.h"
#include "MS_Player.h"
#include "MS_PlayerController.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMS_Weapon::AMS_Weapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;
	
	// 내 코드에서 Root 는 안 만드는 것으로 한다. 
	// Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	// SetRootComponent(Root);
	
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
	
	FirePosition = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FirePosition"));
	FirePosition->SetupAttachment(WeaponMesh);
	
	EjectPosition = CreateDefaultSubobject<USceneComponent>(TEXT("EjectPosition"));
	EjectPosition->SetupAttachment(WeaponMesh);
}


// Called when the game starts or when spawned
void AMS_Weapon::BeginPlay()
{
	Super::BeginPlay();
	
	
}

// Called every frame
void AMS_Weapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMS_Weapon::Fire()
{
	AMS_Player* Player = Cast<AMS_Player>(GetOwner());
	
	// 정상적을 캐스팅 됐는지 확인한다. 
	if (!Player) return ;
	
	AMS_PlayerController* PlayerController = Cast<AMS_PlayerController>(Player->GetController());
	
	// 정상적을 캐스팅 됐는지 확인한다.
	if (!PlayerController) return ;
	
	// 카메라 기준 시점 가져오기
	// 초기화를 직접 해주지 않아도 GetPlayerViewPoint() 에서 채워준다.
	FVector CameraLocation = FVector::ZeroVector;
	FRotator CameraRotation = FRotator::ZeroRotator;
	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
	
	// 카메라 라인트레이스 /// note : 별별별
	const FVector TraceStart = CameraLocation;
	const FVector TraceEnd = TraceStart + CameraRotation.Vector() * BulletMaxDistance;
	
	FHitResult Hit; // 라인트레이스 결과를 담는 결과 저장용 구조체
	FCollisionQueryParams CollisionParams; // 라인트레이스를 어떻게 검사할지 설정하는 옵션 객체
	CollisionParams.AddIgnoredActor(Player);
	CollisionParams.AddIgnoredActor(this); // 무기도 무시함
	
	bool bHit = false;
	
	if (FireShape == EFireShape::Line)
	{
		bHit = GetWorld()->LineTraceSingleByChannel(
			Hit, 
			TraceStart, 
			TraceEnd, 
			ECC_Visibility,
			CollisionParams
		);
	}
	else if(FireShape == EFireShape::Sphere)
	{
		bHit = GetWorld()->SweepSingleByChannel(
			Hit,
			TraceStart,
			TraceEnd,
			FQuat::Identity,
			ECC_Visibility,
			FCollisionShape::MakeSphere(BulletRadius)
		);
	}

	const FVector AimPoint = bHit ? Hit.ImpactPoint : TraceEnd;
	
	FireTowards(AimPoint);
}

// 총구(FirePosition)에서 AimPoint를 향해 발사
void AMS_Weapon::FireTowards(const FVector& AimPoint)
{
	// MS_Player 에 접근해서 현재 조준하고 있는 상태인지 가져오기 위해서 접근한다. 
	AMS_Player* Player = Cast<AMS_Player>(GetOwner());
	
	if (!BulletFactory || !FirePosition || !Player->bWantsADS) return;
	
	const FVector MuzzleLoc = FirePosition->GetComponentLocation();
	const FRotator BulletRot = (AimPoint - MuzzleLoc).Rotation(); 
	const FVector Dir = BulletRot.Vector();
	
	FActorSpawnParameters Params;
	Params.Owner = GetOwner();
	Params.Instigator = Cast<APawn>(GetOwner());
	Params.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* Bullet = GetWorld()->SpawnActor<AActor>(BulletFactory, MuzzleLoc, BulletRot, Params);
	
	if (!Bullet) return;
	
	// 총알 제거 
	Bullet->SetLifeSpan(5.f);
	
	UStaticMeshComponent* mesh = Bullet->FindComponentByClass<UStaticMeshComponent>();
	if (!mesh) return;
	
	// 고속 충돌 안정화
	mesh->BodyInstance.bUseCCD = true;
	
	mesh->SetSimulatePhysics(true); // 물리 충돌 켜기
	mesh->SetEnableGravity(false); // 중력
	mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// 0.02f : 가벼운 탄 / 0.20f : 무거운 탄
	mesh->SetMassOverrideInKg(NAME_None, 0.30f, true);
	// mesh->SetPhysicsLinearVelocity(Dir * 6000.f, false);
	
	
	UE_LOG(LogTemp, Warning, TEXT("Bullet=%s mesh=%s SimPhys=%d Mass=%.4f Collision=%d"),
	*GetNameSafe(Bullet),
	*GetNameSafe(mesh),
	mesh ? mesh->IsSimulatingPhysics() : 0,
	mesh ? mesh->GetMass() : -1.f,
	mesh ? (int32)mesh->GetCollisionEnabled() : -1
	);
	
	// 세부 Collision 설정
	// 내 몸만 무시
	if (AActor* ownerActor = GetOwner())
	{
		// 총알이 움직일때 캐릭터를 무시하고 
		mesh->IgnoreActorWhenMoving(ownerActor, true);
		mesh->MoveIgnoreActors.AddUnique(ownerActor);
		
		// Owner 쪽 캡슐도 총알을 무시하게(더 안정적)
		if (UPrimitiveComponent* OwnerPrim = Cast<UPrimitiveComponent>(ownerActor->GetRootComponent()))
		{
			// 캐릭터가 움직일때 총알을 무시한다. 
			OwnerPrim->IgnoreActorWhenMoving(Bullet, true);
			OwnerPrim->MoveIgnoreActors.AddUnique(Bullet);
		}
	}
		
	// 물리 바디 깨우기
	mesh->WakeAllRigidBodies();
	
	const float ImpulseStrength = 50.f;
	
	// 발사
	// true : 질량은 무시하고 속도 변화처럼 들어간다. 
	// false : 질량의 영향을 받아서 들어간다. 
	mesh->AddImpulse(Dir * ImpulseStrength, NAME_None, false);
	
	
	
	
	DrawDebugSphere(GetWorld(), MuzzleLoc, 6.f, 12, FColor::Red, false, 2.f);
	DrawDebugLine(GetWorld(), MuzzleLoc, AimPoint, FColor::Green, false, 2.f, 0, 1.f);
	DrawDebugLine(GetWorld(), MuzzleLoc, MuzzleLoc + BulletRot.Vector() * 2000.f, FColor::Red, false, 2.f, 0, 1.f);
	
	DrawDebugSphere(GetWorld(), Bullet->GetActorLocation(), 20.f, 12, FColor::Yellow, false, 3.f);
	
	
	FTimerHandle H;
	GetWorld()->GetTimerManager().SetTimer(H, [this, Bullet]()
	{
		if (!IsValid(Bullet)) return;
		DrawDebugSphere(GetWorld(), Bullet->GetActorLocation(), 20.f, 12, FColor::Cyan, false, 3.f);
	}, 0.2f, false);
	const FVector V = mesh->GetPhysicsLinearVelocity();
	
	PlayMuzzleFlash();
	PlayFireSound();
	EjectShell();
}

void AMS_Weapon::PlayMuzzleFlash()
{
	if (!MuzzleFX || !FirePosition) return;

	UNiagaraFunctionLibrary::SpawnSystemAttached(
		MuzzleFX,
		FirePosition,          // FirePosition에 붙임
		NAME_None,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::SnapToTarget,
		true
	);
	
}

void AMS_Weapon::PlayFireSound()
{
	if (!FireSound || !FirePosition) return;
	UGameplayStatics::PlaySoundAtLocation(this, FireSound, FirePosition->GetComponentLocation());
}

void AMS_Weapon::EjectShell()
{
	if (!ShellFactory || !EjectPosition) return;
	
	const FVector Loc = EjectPosition->GetComponentLocation();
	const FRotator Rot = EjectPosition->GetComponentRotation();
	
	AActor* Shell = GetWorld()->SpawnActor<AActor>(ShellFactory, Loc, Rot);
	if (!Shell) return;
	
	if (UStaticMeshComponent* staticMesh = Shell->FindComponentByClass<UStaticMeshComponent>())
	{
		staticMesh->SetSimulatePhysics(true);
		
		// 오른쪽/위로 튀어나가게 (총알의 방향에 맞게 조정 필요하다) // 탄피가 나가는 힘 조정
		const FVector Imp = EjectPosition->GetRightVector() * 5.f + EjectPosition->GetUpVector() * 5.f;
		staticMesh->AddImpulse(Imp, NAME_None, false); // 물리 힘 사용
	}
	Shell->SetLifeSpan(3.f); // 3 초 지정
	
}
