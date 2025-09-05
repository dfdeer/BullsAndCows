#include "BCGameStateBase.h"

#include "Kismet/GameplayStatics.h"
#include "Player/BCPlayerController.h"

void ABCGameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
	if (!HasAuthority())
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			if (ABCPlayerController* BCPC = Cast<ABCPlayerController>(PC))
			{
				FString NotificationString = InNameString + TEXT(" joined the game.");
				BCPC->PrintChatMessageString(NotificationString);
			}
		}
	}
}
