// Fill out your copyright notice in the Description page of Project Settings.


#include "CollectableKey.h"
#include "Net/UnrealNetwork.h"
#include "SubsystemCourseCharacter.h"


// Sets default values
ACollectableKey::ACollectableKey()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Comp"));
	SetRootComponent(RootComp);

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CApsule"));
	Capsule->SetupAttachment(RootComp);
	Capsule->SetIsReplicated(true);
	Capsule->SetCollisionProfileName(FName("OverlapAllDynamic"));
	Capsule->SetCapsuleHalfHeight(150.0f);
	Capsule->SetCapsuleRadius(100.0f);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	Mesh->SetupAttachment(RootComp);
	Mesh->SetIsReplicated(true);
	Mesh->SetCollisionProfileName(FName("OverlapAllDynamic"));

	RotationSpeed = 100.0f;

	CollectAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("CollectAudio"));
	CollectAudio->SetupAttachment(RootComp);
	CollectAudio->SetAutoActivate(false);
}


// Called when the game starts or when spawned
void ACollectableKey::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACollectableKey::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(HasAuthority())
	{
		FRotator Rotator = FRotator(0.0f,RotationSpeed * DeltaTime,0.0f);
		Mesh->AddRelativeRotation(Rotator);

		TArray<AActor*> OverlapingActors;
		Capsule->GetOverlappingActors(OverlapingActors,ASubsystemCourseCharacter::StaticClass());
		if(!OverlapingActors.IsEmpty())
		{
			if(!IsCollected)
			{
				IsCollected = true;
				OnRep_IsCollected();
			}
		}
	}

}


void ACollectableKey::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACollectableKey,IsCollected); // Registering 
}

void ACollectableKey::OnRep_IsCollected()
{
	if(HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Server: OnRep_IsCollected()"));
		if(IsCollected)
		{
			OnCollected.Broadcast();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Client: OnRep_IsCollected()"));
	}
	Mesh->SetVisibility(!IsCollected);
	CollectAudio->Play();

	if(IsCollected)
	{
		if(KeyHolderRef)
		{
			KeyHolderRef->ActiavateKeyMesh();	
		}
	}
}
