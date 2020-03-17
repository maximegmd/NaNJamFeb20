#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GJCharacter.h"
#include "GJWeaponComponent.generated.h"

class AGJWeapon;

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class GAMEJAM_API UGJWeaponComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UGJWeaponComponent();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void Equip(const TSubclassOf<AGJWeapon>& WeaponClass);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void Unequip();

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsEquipped() const;

	UFUNCTION(BlueprintPure, Category = "Character")
	AGJCharacter* GetCharacter() const;

	// Networking
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
private:

	UPROPERTY()
	AGJWeapon* CurrentWeapon;

	UPROPERTY(Replicated)
	bool bEquipped;
};
