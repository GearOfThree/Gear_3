#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GearWeaponBase.generated.h"

UCLASS(Abstract)
class GEAROFTHREE_API AGearWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AGearWeaponBase();

	// 모든 무기가 공통으로 가질 발사 함수 (자식에서 반드시 구현)
	virtual void Fire() PURE_VIRTUAL(AGearWeaponBase::Fire, );

protected:
	// 무기 외형 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	// 총구 위치 (이펙트나 투사체 생성용)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<USceneComponent> MuzzleLocation;
};