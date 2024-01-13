// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Transpoter.h"

#include "PressurePlate.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPressurePlateOnActivated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPressurePlateOnDeactivated);

UCLASS()
class SUBSYSTEMCOURSE_API APressurePlate : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APressurePlate();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite,VisibleAnywhere)
	USceneComponent* RootComp;
	
	UPROPERTY(BlueprintReadWrite,VisibleAnywhere)
	UStaticMeshComponent* TriggerMesh;

	UPROPERTY(BlueprintReadWrite,VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(BlueprintReadWrite,VisibleAnywhere)
	bool Activated;

	UPROPERTY(BlueprintAssignable)
	FPressurePlateOnActivated ActivateDel;

	UPROPERTY(BlueprintAssignable)
	FPressurePlateOnDeactivated DeactivateDel;

	UPROPERTY(BlueprintReadWrite,VisibleAnywhere)
	UTranspoter* Transporter;
};
