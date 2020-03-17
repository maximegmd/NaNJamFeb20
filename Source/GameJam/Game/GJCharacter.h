#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GJCharacterMovementComponent.h"

#include "GJCharacter.generated.h"

class AGJWeapon;

UCLASS(Blueprintable)
class AGJCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AGJCharacter(const FObjectInitializer& ObjectInitializer);

	void Tick(float DeltaSeconds) override;
	void BeginPlay() override;
	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	// Getters
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UGJCharacterMovementComponent* GetCharacterMovementComponent() const { return Cast<UGJCharacterMovementComponent>(GetCharacterMovement()); }
	FORCEINLINE float GetBaseTurnRate() const { return BaseTurnRate; }

	// Interface
	void AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce) override;
	void SetRotation(FRotator Rotation);
	void Jump() override;

	UFUNCTION(BlueprintCallable)
	void ImmediateSimpleAttack();

	UFUNCTION(BlueprintCallable)
	void DrawWeapon();

	// Blueprint exposure
	UFUNCTION(BlueprintPure)
	bool IsWeaponEquipped() const;

	UFUNCTION(BlueprintPure)
	bool WantsSimpleAttack() const;

	UFUNCTION(BlueprintCallable)
	void DashSword();

	UFUNCTION(BlueprintCallable)
	void IncrementAttackActionCount();

	UFUNCTION(BlueprintPure)
	float GetMaximumHealth() const;

	UFUNCTION(BlueprintCallable)
	void PickUpWeapon(const TSubclassOf<AGJWeapon>& AWeaponClass);

	UFUNCTION(BlueprintCallable)
	float Heal();

protected:
	UFUNCTION(Server, Unreliable)
	void ServerSetActorRotation(FRotator Rotation);

	UFUNCTION(Server, Unreliable)
	void ServerJump();

	UFUNCTION(Server, Reliable)
	void ServerDrawWeapon();

	UFUNCTION(Server, Reliable)
	void ServerImmediateSimpleAttack();

	UFUNCTION(Server, Reliable)
	void ServerDashSword();

	// Business code
	bool DoJump(float Distance);
	void DoDrawWeapon();

	// Events
	void Landed(const FHitResult& Hit) override;

	// Networking
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	class UGJWeaponComponent* WeaponComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AGJWeapon> WeaponClass;

	UPROPERTY(Replicated)
	float LastSimpleAttackTimer;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	int32 AttackActionCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	uint32 bPerformingAttack : 1;
};