#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "FindSessionsCallbackProxy.h"
#include "GJGameInstance.generated.h"

UCLASS()
class GAMEJAM_API UGJGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UGJGameInstance(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = Online)
	bool Host(ULocalPlayer* Player, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers);

	UFUNCTION(BlueprintCallable, Category = Online)
	class UDelaySessionFind* FindSessions(ULocalPlayer* Player, bool bIsLAN, bool bIsPresence);

	UFUNCTION(BlueprintCallable, Category = Online)
	void Join(ULocalPlayer* Player, FName SessionName, const FBlueprintSessionResult& SearchResult);

	UFUNCTION(BlueprintImplementableEvent)
	void OnSessionResult(const TArray<FBlueprintSessionResult>& Sessions);

	UFUNCTION(BlueprintImplementableEvent)
	void OnSessionCreated(bool bSuccessful);

protected:

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

private:

	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;

	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	TSharedPtr<class FOnlineSessionSettings> SessionSettings;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;
};