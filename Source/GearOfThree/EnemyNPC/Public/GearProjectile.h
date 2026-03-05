#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GearProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class UNiagaraComponent;

UCLASS()
class GEAROFTHREE_API AGearProjectile : public AActor
{
	GENERATED_BODY()
    
public: 
	AGearProjectile();

protected:
	virtual void BeginPlay() override;

	// 1. 모든 총알이 공통으로 가지는 부품들 (자식들이 그대로 물려받음)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ProjectileMesh; // 💡 BuzzKill의 SawMesh와 Ally의 ProjectileMesh를 이걸로 통일합니다!

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VFX")
	UNiagaraComponent* TrailEffectComp;

	// 충돌 이벤트 (자식들이 각자 입맛에 맞게 재정의(Override) 할 수 있도록 virtual을 붙입니다)
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:
	
	//오브젝트 풀링 (자고 깨어나는 기능)
	bool bIsActive = false;

	// virtual을 붙여서 톱날(BuzzKill)처럼 특이한 물리법칙을 쓰는 애들은 깨어날 때 다른 짓을 할 수 있게 해줍니다.
	virtual void ActivateProjectile(FVector SpawnLocation, FRotator SpawnRotation);
	virtual void DeactivateProjectile();
};