#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GJPlayerController.generated.h"

class AGJCharacter;

UCLASS()
class AGJPlayerController : public APlayerController
{
	GENERATED_BODY()

	FVector2D LatestCursorPosition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|HUD", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> GameHUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|HUD", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> GameEndWidgetClass;

	UPROPERTY()
	class UUserWidget* GameHUDWidget;

public:
	AGJPlayerController();

	void NotifyDeath();

protected:
	// Begin PlayerController interface
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void PlayerTick(float DeltaTime) override;
	void SetupInputComponent() override;
	// End PlayerController interface

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	UFUNCTION(BlueprintCallable, Category = "Character")
	AGJCharacter* GetGJCharacter() const;
	
	void MoveForward(float Value);
	void MoveRight(float Value);

	void TurnAtRate(float Rate);
	void CursorMove(float Value);

	void Jump();
	void DrawWeapon();
	void StartSimpleAttack();
	void StopSimpleAttack();
};