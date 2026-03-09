// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LeechProjectile.generated.h"

UCLASS()
class GEAROFTHREE_API ALeechProjectile : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ALeechProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot|Collision")
	TObjectPtr<class USphereComponent> SphereComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot|Collision")
	TObjectPtr<class UStaticMeshComponent> MeshComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot|Collision")
	TArray<class ALeech*> IgnoreOverlap;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
	FVector Velocity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
	float Gravity = -980.f;
	
	UFUNCTION()
	void Overlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
