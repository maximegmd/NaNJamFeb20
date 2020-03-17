#include "GJGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "DelaySessionFind.h"
#include "Kismet/GameplayStatics.h"

UGJGameInstance::UGJGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UGJGameInstance::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UGJGameInstance::OnStartOnlineGameComplete);
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UGJGameInstance::OnFindSessionsComplete);
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UGJGameInstance::OnJoinSessionComplete);
}

bool UGJGameInstance::Host(ULocalPlayer* Player, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers)
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();

	if (!OnlineSubsystem)
		return false;

	if (!Player)
		Player = GetFirstGamePlayer();

	const auto UserId = Player->GetPreferredUniqueNetId();

	const IOnlineSessionPtr Session = OnlineSubsystem->GetSessionInterface();
	if (!Session.IsValid() || !UserId.IsValid())
		return false;

	SessionSettings = MakeShared<FOnlineSessionSettings>();

	SessionSettings->bIsLANMatch = bIsLAN;
	SessionSettings->bUsesPresence = bIsPresence;
	SessionSettings->NumPublicConnections = MaxNumPlayers;
	SessionSettings->NumPrivateConnections = 0;
	SessionSettings->bAllowInvites = true;
	SessionSettings->bAllowJoinInProgress = true;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;

	SessionSettings->Set(SETTING_MAPNAME, FString("Demonstration"), EOnlineDataAdvertisementType::ViaOnlineService);
	SessionSettings->Set(SETTING_CUSTOMSEARCHINT8, SessionName.ToString(), EOnlineDataAdvertisementType::ViaOnlineService);

	OnCreateSessionCompleteDelegateHandle = Session->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

	return Session->CreateSession(*UserId, SessionName, *SessionSettings);
}

UDelaySessionFind* UGJGameInstance::FindSessions(ULocalPlayer* Player, bool bIsLAN, bool bIsPresence)
{
	UDelaySessionFind* DelaySessionFind = NewObject<UDelaySessionFind>();

	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();

	if (!OnlineSubsystem)
		return DelaySessionFind;

	if (!Player)
		Player = GetFirstGamePlayer();

	const auto UserId = Player->GetPreferredUniqueNetId();

	const IOnlineSessionPtr Session = OnlineSubsystem->GetSessionInterface();
	if (!Session.IsValid() || !UserId.IsValid())
		return DelaySessionFind;

	SessionSearch = MakeShared<FOnlineSessionSearch>();

	SessionSearch->bIsLanQuery = bIsLAN;
	SessionSearch->MaxSearchResults = 20;
	SessionSearch->PingBucketSize = 50;

	if (bIsPresence)
	{
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, bIsPresence, EOnlineComparisonOp::Equals);
	}

	OnFindSessionsCompleteDelegateHandle = Session->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);

	Session->FindSessions(*UserId, SessionSearch.ToSharedRef());

	return DelaySessionFind;
}

void UGJGameInstance::Join(ULocalPlayer* Player, FName SessionName, const FBlueprintSessionResult& SearchResult)
{
	if (!Player)
		Player = GetFirstGamePlayer();

	const auto UserId = Player->GetPreferredUniqueNetId();
	
	const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		const IOnlineSessionPtr Session = OnlineSubsystem->GetSessionInterface();
		if (Session.IsValid() && UserId.IsValid())
		{
			OnJoinSessionCompleteDelegateHandle = Session->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

			Session->JoinSession(*UserId, SessionName, SearchResult.OnlineResult);
		}
	}
}

void UGJGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{	
	const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		const IOnlineSessionPtr Session = OnlineSubsystem->GetSessionInterface();
		if (Session.IsValid())
		{
			Session->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);

			if (bWasSuccessful)
			{
				OnStartSessionCompleteDelegateHandle = Session->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);

				Session->StartSession(SessionName);

				return;
			}
		}
	}

	OnSessionCreated(false);
}

void UGJGameInstance::OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful)
{
	const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		const IOnlineSessionPtr Session = OnlineSubsystem->GetSessionInterface();
		if (Session.IsValid())
		{
			Session->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);

			if (bWasSuccessful)
			{
				OnSessionCreated(true);
				
				UGameplayStatics::OpenLevel(GetWorld(), TEXT("/Game/PolygonScifi/Maps/Demonstration"), true, TEXT("listen"));

				return;
			}
		}
	}

	OnSessionCreated(false);
}

void UGJGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	TArray<FBlueprintSessionResult> Results;
	
	const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	
	if (OnlineSubsystem)
	{
		const IOnlineSessionPtr Session = OnlineSubsystem->GetSessionInterface();
		if (Session.IsValid())
		{
			Session->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);

			if (SessionSearch->SearchResults.Num() > 0)
			{
				for (auto SearchIdx = 0; SearchIdx < SessionSearch->SearchResults.Num(); SearchIdx++)
				{
					const auto CurrentSession = SessionSearch->SearchResults[SearchIdx].Session;

					FBlueprintSessionResult Result;
					Result.OnlineResult = SessionSearch->SearchResults[SearchIdx];
					Results.Add(Result);
				}
			}
		}
		
	}

	OnSessionResult(Results);
}

void UGJGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		const IOnlineSessionPtr Session = OnlineSubsystem->GetSessionInterface();
		if (Session.IsValid())
		{
			Session->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
			
			auto PC = GetFirstLocalPlayerController();

			FString TravelURL;

			if(Session->GetResolvedConnectString(SessionName, TravelURL))
			{
				PC->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
			}
		}
	}
}
