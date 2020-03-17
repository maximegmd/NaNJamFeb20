#include "GJWeapon.h"
#include "Net/UnrealNetwork.h"

AGJWeapon::AGJWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	bReplicates = true;

	ParentCharacter = nullptr;
}

void AGJWeapon::BeginPlay()
{
	Super::BeginPlay();
}

AGJCharacter* AGJWeapon::GetCharacter() const
{
	return ParentCharacter;
}

void AGJWeapon::SetCharacter(AGJCharacter* Character)
{
	ParentCharacter = Character;
}

void AGJWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AGJWeapon, ParentCharacter);
}

void AGJWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

