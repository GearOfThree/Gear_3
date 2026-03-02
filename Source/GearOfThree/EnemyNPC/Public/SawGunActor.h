#pragma once

#include "CoreMinimal.h"
#include "GearWeaponBase.h"
#include "NiagaraSystem.h"
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
	
	// 🔹 총구 화염 이펙트 (블루프린트에서 나이아가라 에셋 지정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	UNiagaraSystem* MuzzleFlashEffect;
};
