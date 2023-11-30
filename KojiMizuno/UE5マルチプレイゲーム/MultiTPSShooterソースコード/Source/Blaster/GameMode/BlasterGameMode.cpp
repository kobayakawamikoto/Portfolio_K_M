// Fill out your copyright notice in the Description page of Project Settings.
// GameMode is called on only server

#include "BlasterGameMode.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Kismet/GamePlayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Blaster/GameState/BlasterGameState.h"
#include "MultiplayerSessionsSubsystem.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldown");

}

ABlasterGameMode::ABlasterGameMode()
{
	bDelayedStart = true;
}

void ABlasterGameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds(); // The time called BeginPlay
}

void ABlasterGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		CountdownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		CountdownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			RestartGame();
			/*
			UGameInstance* GameInstance = GetGameInstance();
			if (GameInstance)
			{
				UMultiplayerSessionsSubsystem* Subsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
				check(Subsystem);

				UWorld* World = GetWorld();
				if (World)
				{
					//bUseSeamlessTravel = true;

					FString MatchType = Subsystem->DesiredMatchType;
					if (MatchType == "FreeForAll")
					{
						World->ServerTravel(FString("/Game/Maps/FreeForAll?listen"));
					}
					else if (MatchType == "Teams")
					{
						World->ServerTravel(FString("/Game/Maps/Teams?listen"));
					}
					else if (MatchType == "CaptureTheFlag")
					{
						World->ServerTravel(FString("/Game/Maps/CaptureTheFlag?listen"));
					}
				}
			}
			*/
		}
	}
}

void ABlasterGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet(); // StartMatch()やSetMatchStateから呼び出される

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABlasterPlayerController* BlasterPlayer = Cast<ABlasterPlayerController>(*It);
		if (BlasterPlayer)
		{
			BlasterPlayer->OnMatchStateSet(MatchState, bTeamsMatch);
		}
	}
}

float ABlasterGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	return BaseDamage;
}

/*
* 死亡時の処理
*/
void ABlasterGameMode::PlayerEliminated(class ABlasterCharacter* ElimmedCharacter, class ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController)
{
	if (AttackerController == nullptr || AttackerController->PlayerState == nullptr) return;
	if (VictimController == nullptr || VictimController->PlayerState == nullptr) return;
	ABlasterPlayerState* AttackerPlayerState = AttackerController ? Cast<ABlasterPlayerState>(AttackerController->PlayerState) : nullptr;
	ABlasterPlayerState* VictimPlayerState = VictimController ? Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;

	ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();
	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState && BlasterGameState)
	{
		TArray<ABlasterPlayerState*> PlayersCurrentlyInTheLead;
		for (auto LeadPlayer : BlasterGameState->TopScoringPlayers)
		{
			PlayersCurrentlyInTheLead.Add(LeadPlayer);
		}

		AttackerPlayerState->AddToScore(1.f);
		BlasterGameState->UpdateTopScore(AttackerPlayerState);

		if (BlasterGameState->TopScoringPlayers.Contains(AttackerPlayerState))
		{
			ABlasterCharacter* Leader = Cast<ABlasterCharacter>(AttackerPlayerState->GetPawn());
			if (Leader)
			{
				Leader->MulticastGainedTheLead();
			}
		}

		for (int32 i = 0; i < PlayersCurrentlyInTheLead.Num(); i++)
		{
			if (!BlasterGameState->TopScoringPlayers.Contains(PlayersCurrentlyInTheLead[i]))
			{
				ABlasterCharacter* Loser = Cast<ABlasterCharacter>(PlayersCurrentlyInTheLead[i]->GetPawn());
				if (Loser)
				{
					Loser->MulticastLostTheLead();
				}
			}
		}
	}
	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}

	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim(false);
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABlasterPlayerController* BlasterPlayer = Cast<ABlasterPlayerController>(*It);
		if (BlasterPlayer && AttackerPlayerState && VictimPlayerState)
		{
			BlasterPlayer->BloadcastElim(AttackerPlayerState, VictimPlayerState);
		}
	}
}

/*
* リスポーン処理
*/
void ABlasterGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}
	if (ElimmedController)
	{
		//UE_LOG(LogTemp, Warning, TEXT("ElimmedController valid"))
		// マップに配置されている全てのプレイヤースタートを取得
		TArray<AActor*>AllPlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), AllPlayerStarts);

		// 全てのプレイヤーを取得
		TArray<AActor*>Characters;
		UGameplayStatics::GetAllActorsOfClass(this, ABlasterCharacter::StaticClass(), Characters);

		TArray<float>StockedDistances;

		for (int i = 0; i < AllPlayerStarts.Num(); i++)
		{
			// キャラクター0(リスポーンする人)とプレイヤースタート[i]との距離を取得
			float MinDistance = (AllPlayerStarts[i]->GetActorLocation() - Characters[0]->GetActorLocation()).Size();
			for (int j = 1; j < Characters.Num(); j++)
			{
				// 'キャラクター0(リスポーンする人)とプレイヤースタート[i]'と'の距離と全てのキャラクターとプレイヤースタート[i]との距離'を比較する
				float Distance = (AllPlayerStarts[i]->GetActorLocation() - Characters[j]->GetActorLocation()).Size();
				if (Distance < MinDistance)
				{
					// 現時点での一番小さな距離が取得される
					MinDistance = Distance;
				}
			}
			// プレイヤースタートの個数分追加するようにする
			// これは次の処理で、何番目のプレイヤースタートが一番離れているかを指定するために必要なため(Selection部分)
			StockedDistances.Add(MinDistance);
		}

		float MaxDistance = StockedDistances[0];
		int32 Selection = 0; // 一番離れているプレイヤースタートを指定するための変数

		for (int i = 1; i < StockedDistances.Num(); i++)
		{
			// StockedDistancesの中で一番値が大きい物を選別
			if (MaxDistance < StockedDistances[i])
			{
				MaxDistance = StockedDistances[i];
				Selection = i;
			}
		}
		RestartPlayerAtPlayerStart(ElimmedController, AllPlayerStarts[Selection]);

		/*
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);*/
	}
}

/*
* プレイヤー退出時の処理
*/
void ABlasterGameMode::PlayerLeftGame(ABlasterPlayerState* PlayerLeaving)
{
	if (PlayerLeaving == nullptr) return;
	ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();
	if (BlasterGameState && BlasterGameState->TopScoringPlayers.Contains(PlayerLeaving))
	{
		BlasterGameState->TopScoringPlayers.Remove(PlayerLeaving);
	}
	ABlasterCharacter* CharacterLeaving = Cast<ABlasterCharacter>(PlayerLeaving->GetPawn());
	if (CharacterLeaving)
	{
		CharacterLeaving->Elim(true);
	}
}