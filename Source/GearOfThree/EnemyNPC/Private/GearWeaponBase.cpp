#include "GearWeaponBase.h"

#include "GearProjectile.h"

AGearWeaponBase::AGearWeaponBase()
{
	// 무기는 보통 매 프레임 Tick이 필요 없으므로 성능을 위해 꺼둡니다.
	PrimaryActorTick.bCanEverTick = false;

	// 1. 무기 메쉬 컴포넌트 생성 및 루트로 설정
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;

	// 2. 총구 위치 컴포넌트 생성 및 메쉬에 부착
	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	MuzzleLocation->SetupAttachment(WeaponMesh);
}

void AGearWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	// 1. 총알 클래스가 블루프린트에서 잘 설정되어 있다면?
	if (ProjectileClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner(); // 무기를 들고 있는 주인을 총알의 주인으로!
		SpawnParams.Instigator = Cast<APawn>(GetOwner());

		// 2. PoolSize(20개) 만큼 반복해서 총알을 미리 만들어 둡니다.
		for (int32 i = 0; i < PoolSize; i++)
		{
			// 허공(0,0,0)에 보이지 않게 생성
			AGearProjectile* NewProj = GetWorld()->SpawnActor<AGearProjectile>(ProjectileClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
            
			if (NewProj)
			{
				// 3. 태어나자마자 바로 잠재워서(Deactivate) 탄약고 배열에 넣습니다.
				NewProj->DeactivateProjectile();
				ProjectilePool.Add(NewProj);
			}
		}
	}
}

AGearProjectile* AGearWeaponBase::GetPooledProjectile()
{
	// 1. 탄약고(배열)를 싹 뒤지면서 쉬고 있는 녀석을 찾습니다.
	for (AGearProjectile* Proj : ProjectilePool)
	{
		// 총알이 존재하고 && 지금 자고 있는(!bIsActive) 상태라면?
		if (Proj && !Proj->bIsActive)
		{
			return Proj; // "너 당첨! 나가서 일해라!" 하고 줍니다.
		}
	}

	// 2. 만약 20발이 전부 날아가고 있어서 빈 총알이 하나도 없다면? (안전 장치)
	// 에러가 나지 않게 실시간으로 하나 더 스폰해서 풀(Pool)을 늘려줍니다.
	if (ProjectileClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = Cast<APawn>(GetOwner());
        
		AGearProjectile* ExtraProj = GetWorld()->SpawnActor<AGearProjectile>(ProjectileClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (ExtraProj)
		{
			ExtraProj->DeactivateProjectile();
			ProjectilePool.Add(ExtraProj);
			return ExtraProj;
		}
	}

	return nullptr;
}