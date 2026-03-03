#pragma once

#include "CoreMinimal.h"
#include "MS_RecoilSpec.generated.h"

USTRUCT(BlueprintType)
struct FMS_RecoilSpec
{
	GENERATED_BODY()
	
	// ===== Camera kick (deg) =====
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Recoil|Camera")
	float PitchKickMin = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Recoil|Camera")
	float PitchKickMax = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Recoil|Camera")
	float YawKickMin = -0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Recoil|Camera")
	float YawKickMax = 0.5f;

	// ===== Accumulation =====
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Recoil|Accum")
	float SustainPerShot = 0.08f;	// 연사 시 점점 강해지는 정도(배수 증가)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Recoil|Accum")
	float SustainDecaySpeed = 8.0f;	// 쏘지 않을 때 배수 복구 속도

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Recoil|Accum")
	float SustainMax = 2.0f;

	// ===== Recover (how fast recoil returns to 0) =====
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Recoil|Recover")
	float ReturnSpeed = 18.0f;

	// ===== ADS multiplier =====
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Recoil|ADS")
	float ADS_Multiplier = 0.6f;

	// ===== Weapon mesh kick =====
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Recoil|WeaponMesh")
	float KickbackDistance = 6.0f;	// cm (뒤로 밀림)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Recoil|WeaponMesh")
	float KickbackUp = 1.0f;		// cm (살짝 위로)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Recoil|WeaponMesh")
	float KickRotPitch = 2.0f;		// deg (살짝 위로 들림)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Recoil|WeaponMesh")
	float WeaponKickInSpeed = 35.0f;   // 튀는 속도

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Recoil|WeaponMesh")
	float WeaponReturnSpeed = 22.0f;   // 복구 속도

	// ===== Camera shake =====
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Recoil|Shake")
	TSubclassOf<class UCameraShakeBase> FireShake = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Recoil|Shake")
	float ShakeScale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Recoil|Shake")
	float ADS_ShakeScale = 0.6f;

	// ===== Optional pattern (ShotIndex-based) =====
	// x = PitchKick, y = YawKick (deg). If empty => random.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Recoil|Pattern")
	TArray<FVector2D> Pattern;
};
