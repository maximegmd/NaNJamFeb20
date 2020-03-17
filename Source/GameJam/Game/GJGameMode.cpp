#include "GJGameMode.h"
#include "GJPlayerController.h"
#include "GJCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/DefaultPawn.h"

AGJGameMode::AGJGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/GameJam/Blueprints/PlayerCharacter"));
	
	bInGame = false;

	PlayerControllerClass = AGJPlayerController::StaticClass();

	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AGJGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	
	if (MapName.Contains(TEXT("MainMenu")))
		bInGame = false;
	else
		bInGame = true;

	if(!bInGame)
	{
		DefaultPawnClass = ADefaultPawn::StaticClass();
		PlayerControllerClass = APlayerController::StaticClass();
	}
}

UClass* AGJGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
 	return Super::GetDefaultPawnClassForController_Implementation(InController);
}
