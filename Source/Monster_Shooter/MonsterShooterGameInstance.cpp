// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterShooterGameInstance.h"
#include "Engine/Engine.h"


UMonsterShooterGameInstance::UMonsterShooterGameInstance()
{
    // Initialize default values
    CurrentLevelName = "PowerPlant";
    TotalEnemies = 5;
}

void UMonsterShooterGameInstance::SaveLevelProgress(FName NewLevelName, int32 NewTotalEnemies) {
    CurrentLevelName = NewLevelName;
    TotalEnemies = NewTotalEnemies;
}

void UMonsterShooterGameInstance::ResetGameProgress() {
    
    CurrentLevelName = "PowerPlant"; 
    TotalEnemies = 5; 
}