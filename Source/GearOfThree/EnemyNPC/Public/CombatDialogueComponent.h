// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatDialogueComponent.generated.h"


USTRUCT(BlueprintType)
struct FDialogueData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	USoundBase* VoiceSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FString SubtitleText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	float Duration = 3.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSubtitleRequested, const FString&, SubtitleText, float ,Duration);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GEAROFTHREE_API UCombatDialogueComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatDialogueComponent();
	
	UPROPERTY(BlueprintAssignable, Category="Dialogue")
	FOnSubtitleRequested OnSubtitleRequested;
	
	UFUNCTION(BlueprintCallable, Category="Dialogue")
	void PlayDialogue(const FDialogueData& DialogueData);

public:
	// 🔹 에디터에서 대사를 순서대로 차곡차곡 넣을 배열 (큐시트)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TArray<FDialogueData> CombatDialogueSequence;

	// 🔹 전투가 시작되었을 때 외부에서 호출할 함수
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StartCombatDialogue();

protected:
	// 🔹 순차 재생을 위한 타이머와 인덱스
	FTimerHandle DialogueTimerHandle;
	int32 CurrentDialogueIndex = 0;

	// 🔹 다음 대사를 재생하는 내부 함수
	void PlayNextDialogueLine();
};
