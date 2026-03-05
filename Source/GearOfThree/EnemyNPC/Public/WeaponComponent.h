#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GearWeaponBase.h" // AGearWeaponBase를 사용하기 위해 필요할 수 있습니다.
#include "WeaponComponent.generated.h"

USTRUCT(BlueprintType)
struct FWeaponData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<AGearWeaponBase> WeaponClass; 
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GEAROFTHREE_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWeaponComponent();
    
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EquipWeapon(TSubclassOf<AGearWeaponBase> WeaponClass, FName SocketName);

	void Fire(); 

	void SetFireRate(float Rate);
	void SetAmmo(int Ammo);
	// 탄창 및 장전 시스템
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Ammo")
	int32 MaxAmmo = 20; // 최대 탄창

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 CurrentAmmo; // 현재 남은 총알

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Ammo")
	float ReloadTime = 2.0f; // 장전 소요 시간

	bool bIsReloading = false; // 현재 장전 중인지 여부

	// 무기 자체의 연사 속도 제어
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Stats")
	float FireRate = 0.1f; // 0.1초에 한 발씩 발사

	
private:
	float LastFireTime = 0.0f; // 마지막으로 쏜 시간 기억

	// 장전 타이머 관리를 위한 함수와 핸들
	void StartReload();
	void EndReload();
	FTimerHandle ReloadTimerHandle;

	UPROPERTY()
	TObjectPtr<AGearWeaponBase> CurrentWeapon;
};
