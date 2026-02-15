#include "C:\Users\PC\Documents\GitHub\Gear_3\Intermediate\Build\Win64\x64\GearOfThreeEditor\Development\UnrealEd\SharedPCH.UnrealEd.Project.ValApi.ValExpApi.Cpp20.h"
#include "GearWeaponBase.h"

// 바로 이 부분이 누락되어 링크 에러가 발생한 것입니다!
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