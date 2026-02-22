#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"
#include "AllyProjectile.generated.h"

UCLASS()
class GEAROFTHREE_API AAllyProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AAllyProjectile();

protected:
	
	virtual void BeginPlay() override;
	// 충돌 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "Collision")
	class USphereComponent* CollisionComp;

	// 외형 (작은 구체나 미사일 형태)
	UPROPERTY(VisibleAnywhere, Category = "Visual")
	class UStaticMeshComponent* ProjectileMesh;

	// 발사체 이동 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "Movement")
	class UProjectileMovementComponent* ProjectileMovement;

public:
	// 총알이 무언가에 부딪혔을 때 호출될 함수
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
public:
	// 🔹 날아가는 궤적 (항상 켜져 있는 컴포넌트)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VFX")
	UNiagaraComponent* TrailEffectComp;

	// 🔹 피격 이펙트들 (블루프린트에서 각각 다른 에셋 할당)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	UNiagaraSystem* EnemyHitEffect; // 적(사이언)이 맞았을 때

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	UNiagaraSystem* WallHitEffect;  // 벽에 맞았을 때
};
