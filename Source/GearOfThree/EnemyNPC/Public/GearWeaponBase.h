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
	
public:
	// 탄퍼짐 각도 (0이면 레이저, 숫자가 클수록 많이 퍼짐)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Weapon|Stats")
	float WeaponSpreadAngle = 4.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon|Sound")
	USoundBase* FireSound;

protected:
	// 어떤 총알을 미리 만들 것인지 블루프린트에서 설정할 변수
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Pool")
	TSubclassOf<class AGearProjectile> ProjectileClass;

	// 🔹 탄약고 (생성된 총알들을 보관하는 배열)
	UPROPERTY()
	TArray<class AGearProjectile*> ProjectilePool;

	// 🔹 미리 만들어둘 총알 개수 (연사력이 빠른 무기면 50~100개)
	UPROPERTY(EditAnywhere, Category = "Weapon|Pool")
	int32 PoolSize = 20;

	// 🔹 탄약고에서 쉬고 있는 총알을 가져오는 함수
	class AGearProjectile* GetPooledProjectile();

	virtual void BeginPlay() override;
};