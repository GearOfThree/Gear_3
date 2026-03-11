#include "STT_FollowPlayer.h"
#include "NPCCharacter.h"
#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"

EStateTreeRunStatus FSTT_FollowPlayer::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
    
	// 시간 누적
	InstanceData.TimeSinceLastUpdate += DeltaTime;

	// 0.1초(1초에 10번)가 안 지났다면 무거운 연산 스킵!
	if (InstanceData.TimeSinceLastUpdate < 0.1f)
	{
		// 스킵하더라도 실패(Failed)가 아니라 "계속 진행 중(Running)"이라고 트리에 알려줘야 합니다.
		return EStateTreeRunStatus::Running; 
	}

	// 0.1초가 지났다면 타이머 초기화 후 아래 로직 실행
	InstanceData.TimeSinceLastUpdate = 0.0f;
	
	ANPCCharacter* Owner = Cast<ANPCCharacter>(Context.GetOwner());
	if (!Owner) return EStateTreeRunStatus::Failed;

	AAIController* AIC = Cast<AAIController>(Owner->GetController());
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(Context.GetWorld(), 0);

	if (!AIC || !Player) return EStateTreeRunStatus::Failed;

	// 플레이어와의 거리 계산
	float Distance = Owner->GetDistanceTo(Player);

	// 목표 지점 계산 (플레이어의 앞 500 unit 지점)
	// GetActorForwardVector() * -1은 뒤쪽 방향을 의미합니다.
	FVector PlayerBackLocation = Player->GetActorLocation() + (Player->GetActorForwardVector() * 500.0f);

	if (Distance > InstanceData.FollowRadius)
	{
		// MoveToActor 대신 MoveToLocation 사용
		// 이제 Actor를 쳐다보지 않고, 계산된 좌표(등 뒤)를 향해 갑니다.
		AIC->MoveToLocation(PlayerBackLocation, InstanceData.StopRadius);
       
		//이동 중에는 이동 방향을 보게 설정 (몸통 회전 자연스럽게)
		Owner->bUseControllerRotationYaw = false; 
		if (Owner->GetCharacterMovement())
		{
			Owner->GetCharacterMovement()->bOrientRotationToMovement = true;
		}
	}
	else if (Distance <= InstanceData.StopRadius)
	{
		if (AIC->GetMoveStatus() != EPathFollowingStatus::Idle)
		{
			AIC->StopMovement();
		}
	}

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTT_FollowPlayer::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// 화면에 초록색으로 "Follow Start" 출력
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("🟢 [State] Follow Player : START"));
	
	// 로그 창에도 남기기
	//UE_LOG(LogTemp, Log, TEXT("State Tree: Enter Follow Player"));

	return EStateTreeRunStatus::Running;
}

void FSTT_FollowPlayer::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// 상태가 끝날 때 (전투로 넘어갈 때)
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("⚪ [State] Follow Player : END"));
}