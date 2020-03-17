#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GJCharacterMovementComponent.generated.h"


UCLASS()
class GAMEJAM_API UGJCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:

	UGJCharacterMovementComponent();

	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Getters
	bool CanAttemptJump() const override;	
	FORCEINLINE float GetLaunchDistance() const { return LaunchDistance; }
	FORCEINLINE float GetSwordDashDistance() const { return SwordDashDistance; }

	// Actions
	void Jump();

	// Network
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	float JumpCooldown;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	float LaunchDistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	float SwordDashDistance;

	UPROPERTY()
	float CurrentJumpCooldown;

	UPROPERTY(Replicated)
	uint32 JumpsAvailable;
};
