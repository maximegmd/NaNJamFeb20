#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GJGameMode.generated.h"

UCLASS(minimalapi)
class AGJGameMode : public AGameModeBase
{
	GENERATED_BODY()

	bool bInGame;

public:
	AGJGameMode();

	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
};



