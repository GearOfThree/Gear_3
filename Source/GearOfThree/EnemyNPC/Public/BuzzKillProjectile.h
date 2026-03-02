#pragma once

#include "CoreMinimal.h"
#include "GearProjectile.h" // 🚨 부모 클래스 헤더 포함!
#include "BuzzKillProjectile.generated.h"

class UNiagaraSystem;

UCLASS()
class GEAROFTHREE_API ABuzzKillProjectile : public AGearProjectile
{
	GENERATED_BODY()
    
public: 
	ABuzzKillProjectile();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// 톱날만의 고유 물리 세팅 (그대로 유지)
	UPROPERTY(EditAnywhere, Category="Physics")
	float LaunchPower = 4000.0f;
    
	UPROPERTY(EditAnywhere, Category="Physics")
	float SpinPower = 100000.0f;
    
	UPROPERTY(EditAnywhere, Category = "Physics")
	float Bounciness = 0.95f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics")
	float CustomGravityScale = 0.3f;
    
	FVector LastFrameVelocity;

	// 부모의 OnHit을 덮어써서 톱날만의 튕기는 로직과 데미지를 구현합니다.
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
    
	virtual void ActivateProjectile(FVector SpawnLocation, FRotator SpawnRotation) override;
	virtual void DeactivateProjectile() override;
public:
	// 🔹 피격 이펙트들
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	UNiagaraSystem* AllyHitEffect;  

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	UNiagaraSystem* BounceSparkEffect;
};
