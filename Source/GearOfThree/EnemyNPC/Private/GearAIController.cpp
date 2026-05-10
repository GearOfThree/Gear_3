#include "GearAIController.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "GearCharacter.h"
#include "AIDebugComponent.h"

void AGearAIController::FindCoverLocation(UEnvQuery* InCoverQuery)
{
	UE_LOG(LogTemp, Log, TEXT("🎯 [STEP 3] GearAIController: FindCoverLocation Started"));

	FEnvQueryRequest QueryRequest(InCoverQuery, GetPawn());
	TWeakObjectPtr<AGearAIController> WeakThis = this;

	FQueryFinishedSignature FinishDelegate = FQueryFinishedSignature::CreateLambda(
	   [WeakThis, this](TSharedPtr<FEnvQueryResult> Result)
	   {
		  if (AGearAIController* ValidThis = WeakThis.Get())
		  {
			 if (Result.IsValid() && Result->IsSuccessful())
			 {
				FVector BestLocation = Result->GetItemAsLocation(0);
				UE_LOG(LogTemp, Warning, TEXT("✅ [STEP 4] EQS Found Cover! Moving to: %s"), *BestLocation.ToString());
			 	
				// 디버그 컴포넌트를 사용하여 EQS 시각화
				if (AGearCharacter* GearChar = Cast<AGearCharacter>(ValidThis->GetPawn()))
				{
					if (GearChar->DebugComp)
					{
						GearChar->DebugComp->VisualizeEQSResults(Result);
						GearChar->DebugComp->SetCurrentState(TEXT("Finding Cover"), TEXT("EQS Success"));
					}
				}

				ValidThis->MoveToLocation(BestLocation);
			 }
			 else
			 {
				UE_LOG(LogTemp, Error, TEXT("❌ [STEP 4] EQS FAILED to find any cover points!"));
			 }
		  }
	   }
	);

	QueryRequest.Execute(EEnvQueryRunMode::SingleResult, FinishDelegate);
}