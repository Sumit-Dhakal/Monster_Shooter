#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"

USTRUCT(BlueprintType)
struct FGridNode
{
    GENERATED_BODY()

        FVector WorldPosition;
    bool bWalkable;

    int32 GCost;
    int32 HCost;
    FGridNode* Parent;

    int32 GridX;
    int32 GridY;

    FGridNode()
        : WorldPosition(FVector::ZeroVector), bWalkable(true), GCost(0), HCost(0), Parent(nullptr), GridX(0), GridY(0) {}

    int32 FCost() const { return GCost + HCost; }
};

UCLASS()
class MONSTER_SHOOTER_API AGridManager : public AActor
{
    GENERATED_BODY()

public:
    AGridManager();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    void DrawGrid();

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
        int32 GridSizeX = 10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
        int32 GridSizeY = 10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
        float NodeSize = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
        bool bShowGrid = true;


    TArray<TArray<FGridNode>> GridNodes;

    int32 GridWidth;
    int32 GridHeight;

    void GenerateGrid();

    FGridNode* GetNodeFromWorldPosition(FVector WorldPosition);
    TArray<FVector> FindPath(FVector StartWorldPos, FVector TargetWorldPos);
    TArray<FGridNode*> GetNeighbors(FGridNode* Node);
    int32 GetDistance(FGridNode* NodeA, FGridNode* NodeB);
    TArray<FVector> RetracePath(FGridNode* StartNode, FGridNode* EndNode);
private:
    bool IsNodeWalkable(FVector WorldPosition);

};
