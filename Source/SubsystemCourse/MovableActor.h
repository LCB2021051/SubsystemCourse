#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Transpoter.h"

#include "MovableActor.generated.h"

UCLASS()
class SUBSYSTEMCOURSE_API AMovableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AMovableActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite,VisibleAnywhere)
	USceneComponent* RootComp;
	
	UPROPERTY(BlueprintReadWrite,VisibleAnywhere)
	UArrowComponent* Point1;

	UPROPERTY(BlueprintReadWrite,VisibleAnywhere)
	UArrowComponent* Point2;

	UPROPERTY(BlueprintReadWrite,VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(BlueprintReadWrite,VisibleAnywhere)
	UTranspoter* Transporter;
};
