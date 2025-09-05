#include "BCGameModeBase.h"
#include "BCGameStateBase.h"
#include "Player/BCPlayerState.h"
#include "Player/BCPlayerController.h"
#include "EngineUtils.h"

void ABCGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	if (ABCPlayerController* BCPC = Cast<ABCPlayerController>(NewPlayer))
	{
		BCPC->NotificationText = FText::FromString(TEXT("Connected to the game server"));
		AllPlayerControllers.Add(BCPC);
		
		if (ABCPlayerState* BCPS = BCPC->GetPlayerState<ABCPlayerState>())
		{
			BCPS->PlayerNameString = TEXT("Player ") + FString::FromInt(AllPlayerControllers.Num());

			if (ABCGameStateBase* BCGS = GetGameState<ABCGameStateBase>())
			{
				BCGS->MulticastRPCBroadcastLoginMessage(BCPS->PlayerNameString);
			}
		}
	}
}

void ABCGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	SecretNumString = GenerateSecretNum();
	UE_LOG(LogTemp, Error, TEXT("%s"), *SecretNumString);
}

void ABCGameModeBase::PrintChatMessageString(ABCPlayerController* InChattingPC, const FString& InChatMessageString)
{
	int32 Idx = InChatMessageString.Len() - 3;
	FString GuessNumString = InChatMessageString.RightChop(Idx);
	if (IsGuessNumString(GuessNumString))
	{
		IncreaseGuessCount(InChattingPC);
		FString JudgetResultString = JudgeResult(SecretNumString, GuessNumString);
		
		for (TActorIterator<ABCPlayerController> It(GetWorld()); It; ++It)
		{
			if (ABCPlayerController* BCPC = *It)
			{
				FString CombinedMessageString = InChatMessageString + TEXT(" -> ") + JudgetResultString;
				BCPC->ClientRPCPrintChatMessageString(CombinedMessageString);

				int32 StrikeCount = FCString::Atoi(*JudgetResultString.Left(1));
				JudgeGame(InChattingPC, StrikeCount);
			}
		}
	}
	else
	{
		for (TActorIterator<ABCPlayerController> It(GetWorld()); It; ++It)
		{
			if (ABCPlayerController* BCPC = *It)
			{
				BCPC->ClientRPCPrintChatMessageString(InChatMessageString);
			}
		}
	}
}

FString ABCGameModeBase::GenerateSecretNum()
{
	TArray<int32> Nums;
	for (int32 i = 0; i < 9; ++i)
	{
		Nums.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Nums = Nums.FilterByPredicate([](int32 Num) { return Num > 0; });

	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		int32 Idx = FMath::RandRange(0, Nums.Num() - 1);
		Result.Append(FString::FromInt(Nums[Idx]));
		Nums.RemoveAt(Idx);
	}
	return Result;
}

bool ABCGameModeBase::IsGuessNumString(const FString& InNumString)
{
	bool bCanPlay = false;
	
	do
	{
		if (InNumString.Len() != 3) break;
		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNumString)
		{
			if (FChar::IsDigit(C) == false || C == '0')
			{
				bIsUnique = false;
				break;
			}
			UniqueDigits.Add(C);
		}
		if (!bIsUnique) break;
		bCanPlay = UniqueDigits.Num() == 3;
	} while (false);
	return bCanPlay;
}

FString ABCGameModeBase::JudgeResult(const FString& InSecretNumString, const FString& InGuessNumString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		if (InSecretNumString[i] == InGuessNumString[i])
		{
			StrikeCount++;
		}
		else
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumString[i]);
			if (InSecretNumString.Contains(PlayerGuessChar))
			{
				BallCount++;
			}
		}
	}
	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}
	return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
}

void ABCGameModeBase::IncreaseGuessCount(ABCPlayerController* InChattingPC)
{
	if (ABCPlayerState* BCPS = InChattingPC->GetPlayerState<ABCPlayerState>())
	{
		BCPS->CurrentGuessCount++;
	}
}

void ABCGameModeBase::JudgeGame(ABCPlayerController* InChattingPC, int InStrikeCount)
{
	if (InStrikeCount == 3)
	{
		if (ABCPlayerState* BCPS = InChattingPC->GetPlayerState<ABCPlayerState>())
		{
			for (const auto& BCPC : AllPlayerControllers)
			{
				FString CombinedMessageString = BCPS->PlayerNameString + TEXT(" won the game.");
				BCPC->NotificationText = FText::FromString(CombinedMessageString);

				ResetGame();
			}
		}
	}
	else
	{
		bool bIsDraw = true;
		for (const auto& BCPC : AllPlayerControllers)
		{
			if (ABCPlayerState* BCPS = BCPC->GetPlayerState<ABCPlayerState>())
			{
				if (BCPS->CurrentGuessCount < BCPS->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}
		if (bIsDraw)
		{
			for (const auto& BCPC : AllPlayerControllers)
			{
				BCPC->NotificationText = FText::FromString(TEXT("Draw"));
				
				ResetGame();
			}
		}
	}
}

void ABCGameModeBase::ResetGame()
{
	SecretNumString = GenerateSecretNum();
	for (const auto& BCPlayerController : AllPlayerControllers)
	{
		if (ABCPlayerState* BCPS = BCPlayerController->GetPlayerState<ABCPlayerState>())
		{
			BCPS->CurrentGuessCount = 0;
		}
	}
}
