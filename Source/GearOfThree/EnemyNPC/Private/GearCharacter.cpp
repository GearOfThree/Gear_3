// Fill out your copyright notice in the Description page of Project Settings.


#include "GearCharacter.h"
#include "GearOfThreeTypes.h"


// Sets default values
AGearCharacter::AGearCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	// 기본 팀은 없음(None)으로 설정
	TeamSide = ETeamSide::None;
}

bool AGearCharacter::IsHostile(AGearCharacter* OtherCharacter) const
{
	// 대상이 없거나 나 자신이면 적이 아님
	if (!OtherCharacter || OtherCharacter == this) return false;

	ETeamSide OtherTeam = OtherCharacter->GetTeamSide();

	// 소속이 없는(None) 대상은 무시
	if (TeamSide == ETeamSide::None || OtherTeam == ETeamSide::None) return false;

	// 같은 팀이면 당연히 적이 아님
	if (TeamSide == OtherTeam) return false;

	// Player와 Ally는 서로 팀이 달라도 적이 아님 (동맹 관계)
	if ((TeamSide == ETeamSide::Player && OtherTeam == ETeamSide::Ally) ||
		(TeamSide == ETeamSide::Ally && OtherTeam == ETeamSide::Player))
	{
		return false;
	}
	
	// 위 조건에 안 걸렸고 서로 팀이 다르다면? 무조건 적(Enemy)임
	return true;
}

void AGearCharacter::ChangeTeamSide(ETeamSide team)
{
	this->TeamSide = team;
}
