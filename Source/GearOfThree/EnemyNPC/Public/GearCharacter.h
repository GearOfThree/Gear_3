// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GearOfThreeTypes.h"
#include "MS_Damageable.h"
#include "GameFramework/Character.h"
#include "GearCharacter.generated.h"

UCLASS()
class GEAROFTHREE_API AGearCharacter : public ACharacter, public IMS_Damageable
{
	GENERATED_BODY()

public:
	// 생성자
	AGearCharacter();

	// 소속 팀 변수 (아군 NPC는 Ally, 적 NPC는 Enemy로 설정, 플레이어는 Player로 설정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
	ETeamSide TeamSide = ETeamSide::None;

	// 🔹 팀 확인용 Getter 함수 (나중에 총알이나 AI가 피아식별할 때 아주 유용하게 쓰입니다)
	UFUNCTION(BlueprintCallable, Category = "Team")
	ETeamSide GetTeamSide() const { return TeamSide; }
	
public:
	// 🔹 피아 식별 함수: 상대방이 나랑 적대적인 관계(Hostile)인지 확인합니다.
	UFUNCTION(BlueprintCallable, Category = "Team")
	bool IsHostile(AGearCharacter* OtherCharacter) const;
};