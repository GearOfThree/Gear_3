// Fill out your copyright notice in the Description page of Project Settings.


#include "MS_Weapon.h"

#include "MS_Player.h"
#include "MS_PlayerController.h"

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
	const FVector TraceEnd = CameraLocation + CameraRotation.Vector() * BulletMaxDistance;
	
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
	if (!BulletFactory || !FirePosition) return;
	
	const FVector MuzzleLoc = FirePosition->GetComponentLocation();
	const FRotator BulletRot = (AimPoint - MuzzleLoc).Rotation(); // Note : 확인확인

	FActorSpawnParameters Params;
	Params.Owner = GetOwner();
	Params.Instigator = Cast<APawn>(GetOwner());
	Params.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	GetWorld()->SpawnActor<AActor>(BulletFactory, MuzzleLoc, BulletRot, Params);
	
	
	DrawDebugSphere(GetWorld(), MuzzleLoc, 6.f, 12, FColor::Red, false, 2.f);
	DrawDebugLine(GetWorld(), MuzzleLoc, AimPoint, FColor::Green, false, 2.f, 0, 1.f);
	DrawDebugLine(GetWorld(), MuzzleLoc, MuzzleLoc + BulletRot.Vector() * 2000.f, FColor::Red, false, 2.f, 0, 1.f);
	
	// 물리엔진 코드
	// if (Bullet)
	// {
	// 	
	// 	
	// }
}