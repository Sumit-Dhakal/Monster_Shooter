// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyAIController.h"
#include "GridManager.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyCharacter.h"
#include "MonsterShooterCharacter.h"
#include "GridManager.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"

void AEnemyAIController::BeginPlay()

{
	Super::BeginPlay();

	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(GetPawn());
	if (!Enemy) return;

	// Set initial movement speed
	Enemy->GetCharacterMovement()->MaxWalkSpeed = 600.0f;

	// Find grid manager and player
	GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	// Initial patrol state
	if (GridManager)
	{
		bSearchForPlayer = true;
		SearchForPlayer();
	}
}




void AEnemyAIController::Tick(float DeltaSeconds)

{
	Super::Tick(DeltaSeconds);

	if (bWaitingAtPoint) return; // Don't move while waiting

	if (bMoveToPlayer && PlayerPawn && GridManager)
	{
		if (IsEnemyCloseToPlayer() && bCanAttackPlayer)
		{
			AttackPlayer();
		}
		else
		{
			MoveToPlayer();
		}
	}
	else if (bSearchForPlayer)
	{
		MoveAlongPath();
	}

	if (bDrawDebugSpheres && CurrentPath.Num() > 0)
	{
		for (int32 i = PathIndex; i < CurrentPath.Num(); i++)
		{
			DrawDebugSphere(
				GetWorld(),
				CurrentPath[i],
				50.f,         // Sphere radius
				8,            // Segments
				FColor::Blue,
				false,        // Persistent
				0.1f          // Lifetime (update every frame)
			);
		}
	}

}




FVector AEnemyAIController::GenerateRandomSearchLocation()
{
	if (!GridManager || !GetPawn()) return FVector::ZeroVector;

	FGridNode* CurrentNode = GridManager->GetNodeFromWorldPosition(GetPawn()->GetActorLocation());
	TArray<FGridNode*> PotentialNodes;

	// Search within 15 nodes radius of current position
	const int SearchRadius = 15;
	for (int32 x = -SearchRadius; x <= SearchRadius; x++)
	{
		for (int32 y = -SearchRadius; y <= SearchRadius; y++)
		{
			if (CurrentNode)
			{
				int32 CheckX = CurrentNode->GridX + x;
				int32 CheckY = CurrentNode->GridY + y;

				if (CheckX >= 0 && CheckX < GridManager->GridSizeX &&
					CheckY >= 0 && CheckY < GridManager->GridSizeY)
				{
					FGridNode* Node = &GridManager->GridNodes[CheckX][CheckY];
					if (Node->bWalkable)
					{
						PotentialNodes.Add(Node);
					}
				}
			}
		}
	}

	if (PotentialNodes.Num() > 0)
	{
		return PotentialNodes[FMath::RandRange(0, PotentialNodes.Num() - 1)]->WorldPosition;
	}

	return GetPawn()->GetActorLocation(); // Fallback to current position
}


void AEnemyAIController::SearchForPlayer()
{
	FlushPersistentDebugLines(GetWorld());
	if (!GridManager || !GetPawn()) return;

	// Generate new patrol destination
	FVector NewPatrolLocation = GenerateRandomSearchLocation();
	CurrentPath = GridManager->FindPath(GetPawn()->GetActorLocation(), NewPatrolLocation);

	if (CurrentPath.Num() > 0)
	{
		PathIndex = 0;
		bSearchForPlayer = true;
		bMoveToPlayer = false;
	}
}

void AEnemyAIController::OnWaitComplete()
{
	bWaitingAtPoint = false;
	SearchForPlayer(); // Get new patrol destination
}
void AEnemyAIController::StartWaitingAtPoint()
{
	if (bWaitingAtPoint) return;

	bWaitingAtPoint = true;

	// Stop movement
	if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(GetPawn()))
	{
		Enemy->GetCharacterMovement()->StopMovementImmediately();
	}

	// Set wait timer (2-4 seconds)
	float WaitTime = FMath::RandRange(1.0f , 2.0f);
	GetWorld()->GetTimerManager().SetTimer(WaitTimerHandle, this,
		&AEnemyAIController::OnWaitComplete, WaitTime, false);
}

void AEnemyAIController::MoveToPlayer()
{
	if (PlayerPawn && GridManager)
	{
		CurrentPath = GridManager->FindPath(GetPawn()->GetActorLocation(), PlayerPawn->GetActorLocation());
		PathIndex = 0;
		MoveAlongPath();
	}
}

void AEnemyAIController::MoveAlongPath()
{
	if (!GetPawn() || CurrentPath.Num() == 0 || PathIndex >= CurrentPath.Num())
	{
		if (bSearchForPlayer)
		{
			// Reached final patrol point
			StartWaitingAtPoint();
		}
		return;
	}

	FVector CurrentLocation = GetPawn()->GetActorLocation();
	FVector TargetLocation = CurrentPath[PathIndex];

	// Calculate movement direction
	FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal2D();
	GetPawn()->AddMovementInput(Direction, 1.0f);

	// Check if reached current path node
	if (FVector::DistXY(CurrentLocation, TargetLocation) < 15.0f)
	{
		PathIndex++;

		// Check if reached final node
		if (PathIndex >= CurrentPath.Num() && bSearchForPlayer)
		{
			StartWaitingAtPoint();
		}
	}
}


// Function to start chasing the player, but only after the scream animation finishes
void AEnemyAIController::StartChasingPlayer()
{
	if (!PlayerPawn || !GridManager) return;

	// If the enemy has already played the scream animation, start chasing
	if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(GetPawn()))
	{
		if (Enemy->bHasPlayedScreamAnimation)
		{
			// Cancel any waiting
			if (bWaitingAtPoint)
			{
				GetWorld()->GetTimerManager().ClearTimer(WaitTimerHandle);
				bWaitingAtPoint = false;
			}

			// Set chase parameters
			bMoveToPlayer = true;
			bSearchForPlayer = false;

			// Update path immediately
			CurrentPath = GridManager->FindPath(GetPawn()->GetActorLocation(), PlayerPawn->GetActorLocation());
			PathIndex = 0;

			// Increase movement speed for chasing
			Enemy->GetCharacterMovement()->MaxWalkSpeed = 600.0f;
		}
	}
}


bool AEnemyAIController::IsEnemyCloseToPlayer()
{
	if (PlayerPawn && GetPawn())
	{
		float DistanceToPlayer = FVector::Dist(PlayerPawn->GetActorLocation(), GetPawn()->GetActorLocation());
		return DistanceToPlayer <= PlayerAttackDistance;
	}
	return false;
}

void AEnemyAIController::AttackPlayer()
{
	if (PlayerPawn && IsEnemyCloseToPlayer())
	{
		AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(GetPawn());
		if (EnemyCharacter)
		{
			// Play attack animation
			EnemyCharacter->PlayAttackAnimation();

			// Deal damage to the player
			AMonsterShooterCharacter* PlayerCharacter = Cast<AMonsterShooterCharacter>(PlayerPawn);
			if (PlayerCharacter)
			{
				// Use the DamageAmount from the enemy
				PlayerCharacter->DealDamage(EnemyCharacter->DamageAmount);

				// Optionally, add an attack cooldown
				bCanAttackPlayer = false;
				FTimerHandle AttackCooldownHandle;
				GetWorld()->GetTimerManager().SetTimer(AttackCooldownHandle, [this]()
					{
						bCanAttackPlayer = true;
					}, 2.0f, false); // 2 seconds cooldown
			}
		}
	}
}


void AEnemyAIController::ResetAttackCooldown()
{
	bCanAttackPlayer = true;
}



void AEnemyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	if (Result.IsSuccess())
	{
		AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(GetPawn());
		if (Enemy)
		{
			Enemy->GetCharacterMovement()->MaxWalkSpeed = 500.0f;  // Reset patrol speed
		}

		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Patrol, this, &AEnemyAIController::SearchForPlayer, 2.0f, false);
	}
}


void AEnemyAIController::OnDetectPlayerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == PlayerPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player detected! AI is now chasing."));

		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Patrol);
		GetWorld()->GetTimerManager().ClearTimer(WaitTimerHandle);
		bWaitingAtPoint = false;

		StartChasingPlayer();
	}


}

void AEnemyAIController::OnDetectPlayerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == PlayerPawn)
	{
		bMoveToPlayer = false;
		bSearchForPlayer = true;

		// Reset movement speed
		if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(GetPawn()))
		{
			Enemy->GetCharacterMovement()->MaxWalkSpeed = 600.0f;
		}

		// Generate new patrol path
		SearchForPlayer();
	}
}
