#include "GearAIController.h"
#include "EnvironmentQuery/EnvQueryManager.h"

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
			 	// BestLocation 지점에 높이 100, 반지름 20인 빨간색 기둥을 5초 동안 그립니다.
				//FVector ColumnTop = BestLocation + FVector(0.f, 0.f, 100.f);
			 	//DrawDebugCylinder(GetWorld(), BestLocation, ColumnTop, 20.f, 12, FColor::Red, false, 5.f);
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