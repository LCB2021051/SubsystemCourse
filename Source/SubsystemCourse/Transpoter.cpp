// Fill out your copyright notice in the Description page of Project Settings.


#include "Transpoter.h"
#include "PressurePlate.h"

// Sets default values for this component's properties
UTranspoter::UTranspoter()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);

	MoveTime = 3.0f;
	ActivatedTriggerCount = 0;

	ArePointsSet = false;
	StartPoint = FVector::Zero();
	EndPoint = FVector::Zero();
}


// Called when the game starts
void UTranspoter::BeginPlay()
{
	Super::BeginPlay();

	if(OwnerIsTriggerActor)
	{
		TriggerActors.Add(GetOwner());
	}

	for(AActor* TA:TriggerActors)
	{
		APressurePlate *PressurePlateActor = Cast<APressurePlate>(TA);
		if(PressurePlateActor)
		{
			PressurePlateActor->ActivateDel.AddDynamic(this,&UTranspoter::OnTriggerActorActivated);
			PressurePlateActor->DeactivateDel.AddDynamic(this,&UTranspoter::OnTriggerActorDeactivated);
			continue;
		}

		ACollectableKey* KeyActor = Cast<ACollectableKey>(TA);
		if(KeyActor) 
		{
			KeyActor->OnCollected.AddDynamic(this,&UTranspoter::OnTriggerActorActivated);
		}
	}	
}

void UTranspoter::OnTriggerActorActivated()
{
	ActivatedTriggerCount++;
	// FString Msg = FString::Printf(TEXT("ActivatedTriggerCount: %d"),ActivatedTriggerCount);
	// GEngine->AddOnScreenDebugMessage(-1,5.0f,FColor::White,Msg);
}

void UTranspoter::OnTriggerActorDeactivated()
{
	ActivatedTriggerCount--;
	// FString Msg = FString::Printf(TEXT("ActivatedTriggerCount: %d"),ActivatedTriggerCount);
	// GEngine->AddOnScreenDebugMessage(-1,5.0f,FColor::White,Msg);
}

// Called every frame
void UTranspoter::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(TriggerActors.Num()>0)
	{		
		AllTriggerActorsTriggered = (ActivatedTriggerCount >= TriggerActors.Num());
		if(AllTriggerActorsTriggered)
		{
			// GEngine->AddOnScreenDebugMessage(-1,5.0f,FColor::White,TEXT("All Trigger Actors Triggered"));
		}
	}

	AActor *OwnerActor = GetOwner();

	if(OwnerActor && OwnerActor->HasAuthority() && ArePointsSet)
	{
		FVector CurrentLocation = OwnerActor->GetActorLocation();
		float Speed = FVector::Distance(StartPoint,EndPoint)/MoveTime;

		FVector TargetLocation = AllTriggerActorsTriggered ? EndPoint : StartPoint;

		if(!CurrentLocation.Equals(TargetLocation))
		{
			FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation,TargetLocation, DeltaTime, Speed);
			OwnerActor->SetActorLocation(NewLocation);
		}
	}
}

void UTranspoter::SetPoints(FVector PointOne, FVector PointTwo)
{
	if(PointOne.Equals(PointTwo)) return;
	StartPoint = PointOne;
	EndPoint = PointTwo;
	ArePointsSet = true;
}

