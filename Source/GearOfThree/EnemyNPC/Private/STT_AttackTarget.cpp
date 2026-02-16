#include "STT_AttackTarget.h"
#include "NPCCharacter.h"
#include "StateTreeExecutionContext.h"
#include "WeaponComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

EStateTreeRunStatus FSTT_AttackTarget::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
	ANPCCharacter* Owner = Cast<ANPCCharacter>(Context.GetOwner());
	if (!Owner) return EStateTreeRunStatus::Failed;

	// 타겟 확인
	if (!Owner->GetCurrentTarget()) 
	{
		return EStateTreeRunStatus::Succeeded; // 타겟이 없으면 종료 (전이 발생)
	}
	
	// 전투 중에는 컨트롤러가 보는 곳(적)을 몸통도 봐야 함
	Owner->bUseControllerRotationYaw = true; 
	Owner->GetCharacterMovement()->bOrientRotationToMovement = false; // 이동 방향 보기 끄기

	// 발사 타이머 체크
	float CurrentTime = Context.GetWorld()->GetTimeSeconds();
	if (CurrentTime - InstanceData.LastFireTime >= InstanceData.FireRate)
	{
		// WeaponComponent를 통해 발사
		if (UWeaponComponent* WeaponComp = Owner->FindComponentByClass<UWeaponComponent>())
		{
			WeaponComp->Fire();
			InstanceData.LastFireTime = CurrentTime;
		}
	}

	return EStateTreeRunStatus::Running;
}
