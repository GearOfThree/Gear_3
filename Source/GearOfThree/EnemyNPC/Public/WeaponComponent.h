// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"

USTRUCT(BlueprintType)
struct FWeaponData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AGearWeaponBase> WeaponClass; // 장착할 무기 클래스
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GEAROFTHREE_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UWeaponComponent();
	
	void Fire(); // 이제 내부에서 CurrentWeapon->Fire()만 호출
	// 무기를 소환하고 손 소켓에 부착하는 함수
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EquipWeapon(TSubclassOf<AGearWeaponBase> WeaponClass, FName SocketName);

private:
	UPROPERTY()
	TObjectPtr<AGearWeaponBase> CurrentWeapon;
};
