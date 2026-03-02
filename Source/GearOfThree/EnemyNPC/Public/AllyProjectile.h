#pragma once

#include "CoreMinimal.h"
#include "GearProjectile.h"
#include "AllyProjectile.generated.h"

class UNiagaraSystem;

UCLASS()
class GEAROFTHREE_API AAllyProjectile : public AGearProjectile // 🚨 AActor 대신 부모 상속!
{
	GENERATED_BODY()
    
protected:
	virtual void BeginPlay() override;
	
	FTimerHandle DeactivateTimerHandle;
public: 
	AAllyProjectile();
	
	// 부모의 OnHit을 덮어써서 샐리만의 데미지 로직을 구현합니다.
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
    
public:
	// 피격 이펙트들
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	UNiagaraSystem* EnemyHitEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	UNiagaraSystem* WallHitEffect;  
	
	virtual void ActivateProjectile(FVector SpawnLocation, FRotator SpawnRotation) override;
	virtual void DeactivateProjectile() override;
};
