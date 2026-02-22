#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
};
