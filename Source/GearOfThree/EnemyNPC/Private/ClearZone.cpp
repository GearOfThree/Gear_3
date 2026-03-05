#include "ClearZone.h"

#include "GearGameInstance.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "TimerManager.h"
#include "GameFramework/Character.h"

AClearZone::AClearZone()
{
    PrimaryActorTick.bCanEverTick = false;

    CollisionZone = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionZone"));
    RootComponent = CollisionZone;
    CollisionZone->SetBoxExtent(FVector(200.0f, 200.0f, 100.0f));
    CollisionZone->SetCollisionProfileName(TEXT("Trigger"));
    CollisionZone->OnComponentBeginOverlap.AddDynamic(this, &AClearZone::OnZoneOverlap);
}

void AClearZone::BeginPlay()
{
    Super::BeginPlay();
}

void AClearZone::OnZoneOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this) return;

    ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    
    if (Cast<ACharacter>(OtherActor) == PlayerCharacter)
    {
        // 중복 실행을 막기 위해 닿자마자 박스 콜리전을 꺼버립니다.
        CollisionZone->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        if (UGearGameInstance* GameInst = Cast<UGearGameInstance>(GetGameInstance()))
        {
            GameInst->bIsGameCleared = true; 
        }
        
        // 플레이어의 카메라 매니저를 가져옵니다.
        APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
        if (CameraManager)
        {
            // 페이드 아웃 시작! 
            // (시작 투명도 0.0 -> 끝 투명도 1.0, 걸리는 시간, 검은색, 오디오도 같이 페이드할지, 홀드할지)
            CameraManager->StartCameraFade(0.0f, 1.0f, FadeOutTime, FLinearColor::Black, true, true);
        }

        // 4. 페이드아웃이 진행되는 시간(FadeOutTime)만큼 기다렸다가 다음 레벨로 넘어갑니다!
        GetWorld()->GetTimerManager().SetTimer(
            LevelTransitionTimerHandle, 
            this, 
            &AClearZone::TransitionToNextLevel, 
            FadeOutTime, 
            false
        );
    }
}

void AClearZone::TransitionToNextLevel()
{
    // 에디터에서 이동할 레벨 이름을 제대로 입력했다면 이동!
    if (!TargetLevelName.IsNone())
    {
        UGameplayStatics::OpenLevel(this, TargetLevelName);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("TargetLevelName이 설정되지 않았습니다! BP_ClearZone에서 레벨 이름을 입력해주세요."));
    }
}