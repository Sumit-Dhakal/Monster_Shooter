// EnemyAIController.h
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "EnemyAIController.generated.h"

UCLASS()
class MONSTER_SHOOTER_API AEnemyAIController : public AAIController
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

protected:
    UPROPERTY()
        APawn* PlayerPawn;

    UPROPERTY()
        class AGridManager* GridManager;

    UPROPERTY()
        TArray<FVector> CurrentPath;
    int32 PathIndex;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
        float PlayerAttackDistance = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
        bool bCanAttackPlayer = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
        bool bMoveToPlayer = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
        bool bSearchForPlayer = true;

    UPROPERTY()
        FVector RandomLocation;
    void ResetAttackCooldown();
    FVector GenerateRandomSearchLocation();

    void SearchForPlayer();
    void MoveToPlayer();
    void MoveAlongPath();

    bool IsEnemyCloseToPlayer();
    void AttackPlayer();
public:
    void StartChasingPlayer();
    virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
        bool bDrawDebugSpheres = true;



    UFUNCTION()
        void OnDetectPlayerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
        void OnDetectPlayerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
private:
    FTimerHandle AttackTimerHandle; // Add this member variable
    FTimerHandle TimerHandle_Patrol;



    bool bWaitingAtPoint;
    FTimerHandle WaitTimerHandle;

    void StartWaitingAtPoint();
    void OnWaitComplete();

};
