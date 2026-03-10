// Fill out your copyright notice in the Description page of Project Settings.


#include "Leech/LeechProjectile.h"

#include "Leech.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ALeechProjectile::ALeechProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SphereComp = CreateDefaultSubobject<USphereComponent>(FName("SphereComp"));
	SetRootComponent(SphereComp);
	SphereComp->SetSphereRadius(20.0f);
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(FName("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ALeechProjectile::Overlap);
}

// Called when the game starts or when spawned
void ALeechProjectile::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("[Projectile] BeginPlay: %s  Owner=%s  Instigator=%s  World=%s"),
	*GetName(),
	*GetNameSafe(GetOwner()),
	*GetNameSafe(GetInstigator()),
	*GetNameSafe(GetWorld()));
	
	TArray<AActor*> FoundActors;
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALeech::StaticClass(), FoundActors);

	IgnoreOverlap.Empty();
	for (AActor* Actor : FoundActors)
	{
		if (ALeech* Leech = Cast<ALeech>(Actor))
		{
			IgnoreOverlap.Add(Leech);
		}
	}
	
	APawn* target = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (!target)
	{
		Destroy();
		return;
	}
	
	Velocity = target->GetActorLocation() - GetActorLocation();
	Velocity /= 1.2f;
	Velocity.Z = 550.0f;
}

// Called every frame
void ALeechProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	Velocity.Z = Velocity.Z + Gravity * DeltaTime;
	SetActorLocation(GetActorLocation() + Velocity * DeltaTime);
	SetActorRotation(UKismetMathLibrary::MakeRotFromXZ(Velocity, GetActorUpVector()));
}

void ALeechProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	Destroy();
}

void ALeechProjectile::Overlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this) return;
	
	// GearCharacter를 상속 받고 있는 대상인지 확인한다.
	AMS_DamageableCharacter* Character = Cast<AMS_DamageableCharacter>(OtherActor);
	if (!Character) return; 
	
	// UMS_Damageable를 구현하고 있는지 확인한다. && TeamSide가 Player인지 확인한다. 
	if (OtherActor->Implements<UMS_Damageable>() && Character->TeamSide == ETeamSide::Player)
	{
		IMS_Damageable::Execute_ReceiveDamage(OtherActor, 15.f, this);
	}
	this->Destroy();
}


