#pragma once

#include "CoreMinimal.h"
#include "GearWeaponBase.h"
#include "SawGunActor.generated.h"

UCLASS()
class GEAROFTHREE_API ASawGunActor : public AGearWeaponBase
{
	GENERATED_BODY()

public:
	ASawGunActor();

	// 부모의 Fire를 오버라이드하여 톱날 발사 구현
	virtual void Fire() override;

protected:
	// 톱날 투사체 클래스
	UPROPERTY(EditAnywhere, Category = "Weapon | Combat")
	TSubclassOf<class ABuzzKillProjectile> ProjectileClass;
};
