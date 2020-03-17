// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GJPickUp.generated.h"

UCLASS()
class GAMEJAM_API AGJPickUp : public AActor
{
	GENERATED_BODY()
	
public:	

	AGJPickUp();

protected:
	
	void BeginPlay() override;

public:
	
	void Tick(float DeltaTime) override;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pickup, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pickup, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* StaticMeshComponent;

};
