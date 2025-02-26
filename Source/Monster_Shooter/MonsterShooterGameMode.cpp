#include "MonsterShooterGameMode.h"
#include "EnemyCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterShooterGameInstance.h" // Include the GameInstance


AMonsterShooterGameMode::AMonsterShooterGameMode()
{
    KillCount = 0;
    TotalEnemies = 5;
}

void AMonsterShooterGameMode::BeginPlay()
{
    Super::BeginPlay();

    UMonsterShooterGameInstance* MonsterShooterGameInstance = Cast<UMonsterShooterGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (!MonsterShooterGameInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("GameInstance is null!"));
        return;
    }

    TotalEnemiesInLevel = CountEnemiesInLevel();
    EnemiesKilled = 0;
    bIsTransitioning = false; // Reset flag for the new level

    // Log level name and enemy count
    FString LevelName = MonsterShooterGameInstance->CurrentLevelName.IsValid()
        ? MonsterShooterGameInstance->CurrentLevelName.ToString()
        : FString("InvalidLevelName");

    UE_LOG(LogTemp, Warning, TEXT("Level: %s | Total Enemies: %d"), *LevelName, TotalEnemiesInLevel);
}

void AMonsterShooterGameMode::IncrementKillCount()
{
    EnemiesKilled++;
    UE_LOG(LogTemp, Warning, TEXT("Kills: %d/%d"), EnemiesKilled, TotalEnemiesInLevel);

    if (EnemiesKilled >= TotalEnemiesInLevel && !bIsTransitioning)
    {
        UE_LOG(LogTemp, Warning, TEXT("All enemies killed! Triggering transition..."));
        TransitionToNextLevel();
    }
}

void AMonsterShooterGameMode::TransitionToNextLevel()
{
    if (bIsTransitioning) return; // Prevent duplicate calls
    bIsTransitioning = true;

    UMonsterShooterGameInstance* MonsterShooterGameInstance = Cast<UMonsterShooterGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (!MonsterShooterGameInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("GameInstance is invalid!"));
        bIsTransitioning = false;
        return;
    }

    FName NextLevelName;
    TSubclassOf<UUserWidget> LoadingWidgetToUse = nullptr;  // Determine which loading screen widget to display

    if (MonsterShooterGameInstance->CurrentLevelName == FName(TEXT("PowerPlant")))
    {
        NextLevelName = FName(TEXT("SecondMap"));
        MonsterShooterGameInstance->SaveLevelProgress(NextLevelName, 4);
        LoadingWidgetToUse = LoadingScreenWidgetClass1; // Assign the widget for PowerPlant -> SecondMap transition
    }
    else if (MonsterShooterGameInstance->CurrentLevelName == FName(TEXT("SecondMap")))
    {
        NextLevelName = FName(TEXT("Demo_Streets"));
        MonsterShooterGameInstance->SaveLevelProgress(NextLevelName, 26);
        LoadingWidgetToUse = LoadingScreenWidgetClass2; // Assign the widget for SecondMap -> Demo_Streets transition
    }
    else if (MonsterShooterGameInstance->CurrentLevelName == FName(TEXT("Demo_Streets")))
    {
        UE_LOG(LogTemp, Warning, TEXT("Game Over! Showing Game Over Screen..."));
        if (GameOverWidgetClass)
        {
            UUserWidget* GameOverWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
            if (GameOverWidget)
            {
                GameOverWidget->AddToViewport();
            }
        }
        return;
    }

    // **Show the appropriate Loading Screen before level transition**
    if (LoadingWidgetToUse)
    {
        UUserWidget* LoadingScreenWidget = CreateWidget<UUserWidget>(GetWorld(), LoadingWidgetToUse);
        if (LoadingScreenWidget)
        {
            LoadingScreenWidget->AddToViewport();
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Opening Level: %s"), *NextLevelName.ToString());

    // **Delay Level Transition to allow Loading Screen to be visible**
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, [NextLevelName]()
        {
            UGameplayStatics::OpenLevel(GWorld, NextLevelName);
        }, 2.5f, false); // **2.5 seconds delay before transition**
}




int32 AMonsterShooterGameMode::CountEnemiesInLevel()
{
    TArray<AActor*> Enemies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyCharacter::StaticClass(), Enemies);
    UE_LOG(LogTemp, Warning, TEXT("Found %d enemies in level."), Enemies.Num()); // Debug log
    return Enemies.Num();
}
