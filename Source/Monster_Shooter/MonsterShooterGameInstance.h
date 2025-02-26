// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MonsterShooterGameInstance.generated.h"

UCLASS()
class MONSTER_SHOOTER_API UMonsterShooterGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UMonsterShooterGameInstance();


   public:
    // Current Level Name (Blueprint accessible)
    UPROPERTY(BlueprintReadWrite, Category = "Game Progress")
    FName CurrentLevelName;

    // Total enemies in current level
    UPROPERTY(BlueprintReadWrite, Category = "Game Progress")
    int32 TotalEnemies;

    // Reset game progress
    UFUNCTION(BlueprintCallable, Category = "Game Progress")
        void ResetGameProgress();
   

    // Save level progress
    UFUNCTION(BlueprintCallable, Category = "Progress")
        void SaveLevelProgress(FName NewLevelName, int32 NewTotalEnemies);
};
