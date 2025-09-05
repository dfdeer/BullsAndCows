#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BCPlayerState.generated.h"

UCLASS()
class BULLSANDCOWS_API ABCPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ABCPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	FString GetPlayerInfoString();
public:
	UPROPERTY(Replicated)
	FString PlayerNameString;

	UPROPERTY(Replicated)
	int32 CurrentGuessCount;
	int32 MaxGuessCount;
};
