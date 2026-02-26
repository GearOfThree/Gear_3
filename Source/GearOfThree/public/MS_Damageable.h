
#pragma once
#include "UObject/Interface.h"
#include "MS_Damageable.generated.h"

UINTERFACE(BlueprintType)
class UMS_Damageable :public UInterface
{
	GENERATED_BODY()	
};

class IMS_Damageable
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ReceiveDamage(float Damage, AActor* DamageCauser);
	
	
};
