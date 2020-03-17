#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "DelaySessionFind.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDelaySessionFindPin);

UCLASS()
class GAMEJAM_API UDelaySessionFind : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintAssignable)
	FDelaySessionFindPin SessionsFound;
};
