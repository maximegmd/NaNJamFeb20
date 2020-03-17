// Fill out your copyright notice in the Description page of Project Settings.


#include "GJPickUp.h"
#include "Engine/World.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AGJPickUp::AGJPickUp()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative, false);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionShape"));
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SetRootComponent(SphereComponent);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	StaticMeshComponent->AttachToComponent(SphereComponent, Rules);
	
}

void AGJPickUp::BeginPlay()
{
	Super::BeginPlay();
}

void AGJPickUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const auto Offset = FMath::Sin(GetWorld()->GetTimeSeconds() * 2.5f);
	
	AddActorLocalOffset(FVector(0.f, 0.f, 0.5f) * Offset);
	AddActorLocalRotation(FRotator(0.f, DeltaTime * 100.f, 0.f));
}

