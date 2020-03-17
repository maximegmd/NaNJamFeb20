#include "GJWeaponComponent.h"
#include "GJWeapon.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

UGJWeaponComponent::UGJWeaponComponent()
{
	bEquipped = false;
	CurrentWeapon = nullptr;

	SetIsReplicatedByDefault(true);
	SetIsReplicated(true);
}

void UGJWeaponComponent::Equip(const TSubclassOf<AGJWeapon>& WeaponClass)
{
	if (!WeaponClass)
		return;

	Unequip();
	
	const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, false);

	CurrentWeapon = Cast<AGJWeapon>(GetWorld()->SpawnActor(*WeaponClass));
	if (!CurrentWeapon)
		return;

	CurrentWeapon->SetCharacter(GetCharacter());
	CurrentWeapon->AttachToComponent(this, AttachmentTransformRules);

	bEquipped = true;
}

void UGJWeaponComponent::Unequip()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy(true);
		CurrentWeapon = nullptr;
	}
	
	bEquipped = false;
}

bool UGJWeaponComponent::IsEquipped() const
{
	return bEquipped;
}

AGJCharacter* UGJWeaponComponent::GetCharacter() const
{
	return Cast<AGJCharacter>(GetAttachmentRootActor());
}

void UGJWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UGJWeaponComponent, bEquipped);
}
