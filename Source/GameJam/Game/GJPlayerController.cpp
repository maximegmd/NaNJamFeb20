#include "GJPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "GJCharacter.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "GJGameInstance.h"
#include "Kismet/GameplayStatics.h"

AGJPlayerController::AGJPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
	bEnableClickEvents = false;
	
	static ConstructorHelpers::FClassFinder<UUserWidget> DefaultGameHUDWidgetClass(TEXT("/Game/GameJam/HUD/GameWidget"));
	if (DefaultGameHUDWidgetClass.Class != nullptr)
	{
		GameHUDWidgetClass = DefaultGameHUDWidgetClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> DefaultEndHUDWidgetClass(TEXT("/Game/GameJam/HUD/EndGameWidget"));
	if (DefaultEndHUDWidgetClass.Class != nullptr)
	{
		GameEndWidgetClass = DefaultEndHUDWidgetClass.Class;
	}

	GameHUDWidget = nullptr;
}

void AGJPlayerController::NotifyDeath()
{
	if (IsLocalPlayerController())
	{
		UGameplayStatics::SetGamePaused(GetWorld(), true);

		if (GameHUDWidget)
		{
			GameHUDWidget->RemoveFromViewport();
		}

		if (GameEndWidgetClass)
		{
			auto GameEndWidget = CreateWidget<UUserWidget>(this, *GameEndWidgetClass);
			GameEndWidget->AddToViewport();
			GameEndWidget->SetVisibility(ESlateVisibility::Visible);
			GameEndWidget->bIsFocusable = true;
			
			FInputModeUIOnly InputMode;
			
			SetInputMode(InputMode);
		}
	}
}

void AGJPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	GetMousePosition(LatestCursorPosition.X, LatestCursorPosition.Y);

	const auto GI = Cast<UGJGameInstance>(GetGameInstance());

	FInputModeGameOnly InputMode;
	InputMode.SetConsumeCaptureMouseDown(false);
	
	SetInputMode(InputMode);
	
	if(IsLocalPlayerController() && GameHUDWidgetClass && !GameHUDWidget)
	{
		GameHUDWidget = CreateWidget<UUserWidget>(this, *GameHUDWidgetClass);
		if (!GameHUDWidget)
			return;
	}

	if (GameHUDWidget)
	{
		GameHUDWidget->AddToViewport();
		GameHUDWidget->SetVisibility(ESlateVisibility::Visible);
		GameHUDWidget->bIsFocusable = false;
	}
}

void AGJPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (GameHUDWidget)
	{
		GameHUDWidget->RemoveFromViewport();
	}
}

void AGJPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
}

void AGJPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("MoveForward", this, &AGJPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AGJPlayerController::MoveRight);

	InputComponent->BindAction("Jump", IE_Pressed, this, &AGJPlayerController::Jump);
	InputComponent->BindAction("DrawWeapon", IE_Pressed, this, &AGJPlayerController::DrawWeapon);
	InputComponent->BindAction("SimpleAttack", IE_Pressed, this, &AGJPlayerController::StartSimpleAttack);

	InputComponent->BindAxis("TurnRate", this, &AGJPlayerController::TurnAtRate);
	InputComponent->BindAxis("CursorMove", this, &AGJPlayerController::CursorMove);

	InputComponent->BindAction("ResetVR", IE_Pressed, this, &AGJPlayerController::OnResetVR);
}

void AGJPlayerController::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

AGJCharacter* AGJPlayerController::GetGJCharacter() const
{
	return Cast<AGJCharacter>(GetCharacter());
}

void AGJPlayerController::MoveForward(float Value)
{
	auto TheCharacter = GetGJCharacter();
	
	if (Value != 0.f && TheCharacter)
	{
		// find out which way is forward
		const FRotator Rotation = TheCharacter->GetActorRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		TheCharacter->AddMovementInput(Direction, Value, false);
	}
}

void AGJPlayerController::MoveRight(float Value)
{
	auto TheCharacter = GetGJCharacter();
	
	if (Value != 0.f && TheCharacter)
	{
		// find out which way is forward
		const FRotator Rotation = TheCharacter->GetActorRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		TheCharacter->AddMovementInput(Direction, Value, false);
	}
}

void AGJPlayerController::TurnAtRate(float Rate)
{
	auto TheCharacter = GetGJCharacter();
	
	if (TheCharacter)
	{
		auto Rotation = TheCharacter->GetActorRotation();
		Rotation.Add(0.f, Rate * TheCharacter->GetBaseTurnRate() * GetWorld()->GetDeltaSeconds(), 0.f);

		TheCharacter->SetRotation(Rotation);
	}
}

void AGJPlayerController::CursorMove(float Value)
{
	if (GetWorld()->IsPaused())
		return;
	
	auto TheCharacter = GetGJCharacter();
	
	if (TheCharacter)
	{
		FVector2D CursorPosition;
		GetMousePosition(CursorPosition.X, CursorPosition.Y);

		if (CursorPosition != LatestCursorPosition)
		{
			LatestCursorPosition = CursorPosition;

			int32_t ViewportX, ViewportY;
			GetViewportSize(ViewportX, ViewportY);

			FVector2D Center(static_cast<float>(ViewportX), static_cast<float>(ViewportY));
			Center /= 2.f;

			const auto Direction = CursorPosition - Center;

			auto Rotation = FVector(Direction.X, Direction.Y, 0.f).ToOrientationRotator();
			Rotation.Yaw += 90.f;

			TheCharacter->SetRotation(Rotation);
		}
	}
}

void AGJPlayerController::Jump()
{
	auto TheCharacter = GetGJCharacter();
	
	if (TheCharacter)
	{
		TheCharacter->Jump();
	}
}

void AGJPlayerController::DrawWeapon()
{
	auto TheCharacter = GetGJCharacter();

	if(TheCharacter)
	{
		TheCharacter->DrawWeapon();
	}
}

void AGJPlayerController::StartSimpleAttack()
{
	auto TheCharacter = GetGJCharacter();

	if (TheCharacter)
	{
		TheCharacter->ImmediateSimpleAttack();
	}
}

void AGJPlayerController::StopSimpleAttack()
{
	/*auto TheCharacter = GetGJCharacter();

	if (TheCharacter)
	{
		TheCharacter->ImmediateSimpleAttack();
	}*/
}