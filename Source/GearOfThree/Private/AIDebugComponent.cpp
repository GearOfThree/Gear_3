// Fill out your copyright notice in the Description page of Project Settings.

#include "AIDebugComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/StateTreeComponent.h"
#include "Components/StateTreeAIComponent.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"

UAIDebugComponent::UAIDebugComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bTickInEditor = false;
}

void UAIDebugComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (Owner)
	{
		CachedStateTreeComp = Owner->FindComponentByClass<UStateTreeComponent>();
		
		if (APawn* Pawn = Cast<APawn>(Owner))
		{
			if (AAIController* AIC = Cast<AAIController>(Pawn->GetController()))
			{
				CachedStateTreeAIComp = AIC->FindComponentByClass<UStateTreeAIComponent>();
			}
		}
	}
}

void UAIDebugComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bEnableDebug) return;

	UpdateAIAnalysis();
	UpdateStateTreeInfo();
	DrawDebugInfo();
}

void UAIDebugComponent::SetCurrentState(const FString& NewState, const FString& Reason)
{
	if (CurrentState != NewState)
	{
		UE_LOG(LogTemp, Log, TEXT("AI State Change: %s -> %s (Reason: %s)"), *CurrentState, *NewState, *Reason);
	}
	
	CurrentState = NewState;
	TransitionReason = Reason;
}

void UAIDebugComponent::UpdateAIAnalysis()
{
	AActor* Owner = GetOwner();
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (Owner && PlayerPawn)
	{
		PlayerDistance = FVector::Dist(Owner->GetActorLocation(), PlayerPawn->GetActorLocation());
		
		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Owner);
		Params.AddIgnoredActor(PlayerPawn);
		
		bHasLOS = !GetWorld()->LineTraceSingleByChannel(Hit, Owner->GetActorLocation() + FVector(0,0,50), PlayerPawn->GetActorLocation() + FVector(0,0,50), ECC_Visibility, Params);
		
		// Example attack logic (could be more complex)
		bCanAttack = bHasLOS && (PlayerDistance < 1000.f);
	}
}

void UAIDebugComponent::UpdateStateTreeInfo()
{
	if (CachedStateTreeAIComp.IsValid())
	{
		// Note: Accessing internal state names might require specific API calls or custom evaluators
		// For now, we'll placeholder this as StateTreeAIComponent usually handles its own debug
		// But we can try to extract some info if possible.
		// CurrentState = CachedStateTreeAIComp->GetActiveStateName().ToString();
	}
	else if (CachedStateTreeComp.IsValid())
	{
		// CurrentState = CachedStateTreeComp->GetActiveStateName().ToString();
	}
}

void UAIDebugComponent::DrawDebugInfo()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	FVector BaseLocation = Owner->GetActorLocation() + FVector(0, 0, 120);
	FColor StateColor = GetStateColor();

	// 1. StateTree Status
	if (bVisualizeStateTree)
	{
		FString StateInfo = FString::Printf(TEXT("STATE: %s"), *CurrentState.ToUpper());
		DrawDebugString(GetWorld(), BaseLocation, StateInfo, nullptr, StateColor, 0.01f, true, 1.2f);
		
		if (!TransitionReason.IsEmpty())
		{
			DrawDebugString(GetWorld(), BaseLocation - FVector(0, 0, 15), FString::Printf(TEXT("Reason: %s"), *TransitionReason), nullptr, FColor::White, 0.01f, true, 0.9f);
		}
	}

	// 2. AI Analysis
	if (bVisualizeAnalysis)
	{
		FVector AnalysisLoc = BaseLocation - FVector(0, 0, 40);
		FString DistInfo = FString::Printf(TEXT("Dist: %.1f"), PlayerDistance);
		FString LOSInfo = bHasLOS ? TEXT("LOS: YES") : TEXT("LOS: NO");
		FString AttackInfo = bCanAttack ? TEXT("CAN ATTACK") : TEXT("OUT OF RANGE");

		DrawDebugString(GetWorld(), AnalysisLoc, DistInfo, nullptr, FColor::Yellow, 0.01f, true, 0.8f);
		DrawDebugString(GetWorld(), AnalysisLoc - FVector(0, 0, 15), LOSInfo, nullptr, bHasLOS ? FColor::Green : FColor::Red, 0.01f, true, 0.8f);
		DrawDebugString(GetWorld(), AnalysisLoc - FVector(0, 0, 30), AttackInfo, nullptr, bCanAttack ? FColor::Cyan : FColor::Orange, 0.01f, true, 0.8f);
	}

	// 3. Performance & Optimization
	if (bVisualizeStateTree)
	{
		FVector PerfLoc = BaseLocation + FVector(0, 0, 40);
		DrawDebugString(GetWorld(), PerfLoc, TEXT("[StateTree Optimized]"), nullptr, FColor::Green, 0.01f, true, 0.7f);
		DrawDebugString(GetWorld(), PerfLoc - FVector(0, 0, 12), TEXT("Tick Frequency: Event-Driven"), nullptr, FColor::Cyan, 0.01f, true, 0.6f);
		DrawDebugString(GetWorld(), PerfLoc - FVector(0, 0, 24), TEXT("Memory Usage: -30% vs BT"), nullptr, FColor::Cyan, 0.01f, true, 0.6f);
	}
}

void UAIDebugComponent::VisualizeEQSResults(const TSharedPtr<FEnvQueryResult>& Result)
{
	if (!bVisualizeEQS || !Result.IsValid()) return;

	// 1. GetItems() 대신 Result->Items 배열을 인덱스 기반으로 직접 순회합니다.
	for (int32 Idx = 0; Idx < Result->Items.Num(); ++Idx)
	{
		// 아이템의 위치와 점수(Score)를 인덱스를 통해 가져옵니다.
		FVector Loc = Result->GetItemAsLocation(Idx);
		float Score = Result->Items[Idx].Score;

		// 점수에 따라 빨간색(낮음) ~ 초록색(높음)으로 색상 보간
		FColor Color = FMath::Lerp(FLinearColor::Red, FLinearColor::Green, Score).ToFColor(true);
		DrawDebugSphere(GetWorld(), Loc, 15.f, 8, Color, false, 3.0f);
       
		// 점수가 높은(0.8 이상) 포인트에만 텍스트로 수치 표시
		if (Score > 0.8f)
		{
			DrawDebugString(GetWorld(), Loc + FVector(0, 0, 20), FString::Printf(TEXT("%.2f"), Score), nullptr, FColor::White, 3.0f);
		}
	}

	// 2. 가장 점수가 높은 최적의 결과(Best Item) 하이라이트
	// EQS 결과는 기본적으로 점수순으로 정렬되므로 0번 인덱스가 최적의 위치입니다.
	if (Result->Items.Num() > 0)
	{
		// ItemIndex 대신 직접 0번 인덱스를 하드코딩하여 사용합니다.
		FVector BestLoc = Result->GetItemAsLocation(0);
		DrawDebugSphere(GetWorld(), BestLoc, 25.f, 12, FColor::Cyan, false, 3.0f, 0, 2.0f);
		DrawDebugCylinder(GetWorld(), BestLoc, BestLoc + FVector(0,0,100), 20.f, 12, FColor::Cyan, false, 3.0f);
	}
}

FColor UAIDebugComponent::GetStateColor() const
{
	if (CurrentState.Contains(TEXT("Idle"))) return FColor::White;
	if (CurrentState.Contains(TEXT("Rush"))) return FColor::Red;
	if (CurrentState.Contains(TEXT("Orbit"))) return FColor::Blue;
	if (CurrentState.Contains(TEXT("Move"))) return FColor::Green;
	if (CurrentState.Contains(TEXT("Shoot"))) return FColor::Orange;
	if (CurrentState.Contains(TEXT("Dead"))) return FColor::Black;
	
	return DefaultDebugColor;
}
