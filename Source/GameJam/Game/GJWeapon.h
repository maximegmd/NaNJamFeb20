#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GJCharacter.h"
#include "GJWeapon.generated.h"

UCLASS()
class GAMEJAM_API AGJWeapon : public AActor
{
	GENERATED_BODY()

	UPROPERTY(Replicated)
	AGJCharacter* ParentCharacter;
	
public:	
	AGJWeapon();

	UFUNCTION(BlueprintPure, Category = "Character")
	AGJCharacter* GetCharacter() const;

	void SetCharacter(AGJCharacter* Character);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	void BeginPlay() override;

public:	

	void Tick(float DeltaTime) override;

};
