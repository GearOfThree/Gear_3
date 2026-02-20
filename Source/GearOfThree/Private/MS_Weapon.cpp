// Fill out your copyright notice in the Description page of Project Settings.


#include "MS_Weapon.h"

#include "MS_Player.h"

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
	if (!BulletFactory || !FirePosition) return;
	
	FTransform SpawnBullet = FirePosition->GetComponentTransform();
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride 
		= ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	AMS_Bullet* Bullet = GetWorld()->SpawnActor<AMS_Bullet>(
		BulletFactory,
		SpawnBullet,
		SpawnParams
	);
	
	
	// 물리엔진 코드
	if (Bullet)
	{
		
		
	}
}

