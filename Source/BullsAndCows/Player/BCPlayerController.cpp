#include "BCPlayerController.h"

#include "BCPlayerState.h"
#include "UI/BCChatInput.h"
#include "BullsAndCows.h"
#include "Kismet/GameplayStatics.h"
#include "Game/BCGameModeBase.h"
#include "Net/UnrealNetwork.h"

ABCPlayerController::ABCPlayerController()
{
	bReplicates = true;
}

void ABCPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController()) return;
	
	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);

	if (IsValid(ChatInputWidgetClass))
	{
		ChatInputWidgetInstance = CreateWidget<UBCChatInput>(this, ChatInputWidgetClass);
		if (IsValid(ChatInputWidgetInstance))
		{
			ChatInputWidgetInstance->AddToViewport();
		}
	}

	if (IsValid(NotificationTextWidgetClass))
	{
		if (NotificationTextWidgetInstance = CreateWidget<UUserWidget>(this, NotificationTextWidgetClass))
		{
			NotificationTextWidgetInstance->AddToViewport();
		}
	}
}

void ABCPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;
	if (IsLocalController())
	{
		if (ABCPlayerState* BCPS = GetPlayerState<ABCPlayerState>())
		{
			FString CombinedMessageString = BCPS->GetPlayerInfoString() + TEXT(": ") + InChatMessageString;
			ServerRPCPrintChatMessageString(CombinedMessageString);
		}
	}
}

void ABCPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	BCFucnctionLibrary::MyPrintString(this, InChatMessageString, 10.f);
}

void ABCPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NotificationText);
}

void ABCPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	PrintChatMessageString(InChatMessageString);
}

void ABCPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	if (AGameModeBase* GM = UGameplayStatics::GetGameMode(this))
	{
		if (ABCGameModeBase* BCGM = Cast<ABCGameModeBase>(GM))
		{
			BCGM->PrintChatMessageString(this, InChatMessageString);
		}
	}
}