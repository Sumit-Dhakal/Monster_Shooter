// MonsterShooterGameMode.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MonsterShooterGameMode.generated.h"


UCLASS()
class MONSTER_SHOOTER_API AMonsterShooterGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AMonsterShooterGameMode();

    // Track total enemies and kills for the current level
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Game Stats")
        int32 TotalEnemiesInLevel;

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Game Stats")
        int32 EnemiesKilled;

    // Current kill count
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Game Stats")
        int32 KillCount;

    // Total enemies in current level
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Game Stats")
        int32 TotalEnemies;

    // Game Over UI widget class
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
        TSubclassOf<class UUserWidget> GameOverWidgetClass;

    // Increment kill count and check level completion
    UFUNCTION(BlueprintCallable, Category = "Game Progress")
        void IncrementKillCount();

    // Transition to next level or show Game Over
    UFUNCTION(BlueprintCallable, Category = "Game Progress")
        void TransitionToNextLevel();

protected:
    virtual void BeginPlay() override;

private:
    // Reference to GameInstance
    class UMonsterShooterGameInstance* GameInstance;


protected:

    // Flag to prevent multiple transitions
    bool bIsTransitioning;

private:
    // Function to count enemies at the start of the level
    int32 CountEnemiesInLevel();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
        TSubclassOf<UUserWidget> LoadingScreenWidgetClass1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
        TSubclassOf<UUserWidget> LoadingScreenWidgetClass2;
};