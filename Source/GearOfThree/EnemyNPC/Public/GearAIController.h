#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GearAIController.generated.h"

UCLASS()
class GEAROFTHREE_API AGearAIController : public AAIController
{
	GENERATED_BODY()

public:
	// 🔹 2. 체력이 깎이거나 장전이 필요할 때 호출할 엄폐 함수
	UFUNCTION(BlueprintCallable, Category = "AI | Cover")
	void FindCoverLocation(UEnvQuery* CoverQuery);
};