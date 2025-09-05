#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BCGameModeBase.generated.h"

class ABCPlayerController;

UCLASS()
class BULLSANDCOWS_API ABCGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void OnPostLogin(AController* NewPlayer) override;
	virtual void BeginPlay() override;
	
	void PrintChatMessageString(ABCPlayerController* InChattingPC,  const FString& InChatMessageString);
	FString GenerateSecretNum();
	bool IsGuessNumString(const FString& InNumString);
	FString JudgeResult(const FString& InSecretNumString, const FString& InGuessNumString);

	void IncreaseGuessCount(ABCPlayerController* InChattingPC);

	void JudgeGame(ABCPlayerController* InChattingPC, int InStrikeCount);
	void ResetGame();
protected:
	FString SecretNumString;
	TArray<TObjectPtr<ABCPlayerController>> AllPlayerControllers;
};
