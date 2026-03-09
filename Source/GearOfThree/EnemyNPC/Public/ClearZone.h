#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ClearZone.generated.h"

UCLASS()
class GEAROFTHREE_API AClearZone : public AActor
{
	GENERATED_BODY()
    
public:    
	AClearZone();

protected:
	virtual void BeginPlay() override;

public:
	// 🔹 플레이어 감지용 박스
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* CollisionZone;

	// 🔹 이동할 종료 화면 레벨(맵)의 정확한 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Transition")
	FName TargetLevelName;

	// 🔹 화면이 까맣게 변하는 데 걸리는 시간 (예: 2초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Transition")
	float FadeOutTime = 2.0f;

	UFUNCTION()
	void OnZoneOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	// 🔹 페이드 아웃이 끝난 후 실제로 레벨을 이동시킬 함수
	void TransitionToNextLevel();

	FTimerHandle LevelTransitionTimerHandle;
};