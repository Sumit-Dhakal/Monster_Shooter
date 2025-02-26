#include "GridManager.h"
#include "DrawDebugHelpers.h"

AGridManager::AGridManager()
{
    PrimaryActorTick.bCanEverTick = true;
}
void AGridManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bShowGrid)
    {
        DrawGrid();
    }
}

void AGridManager::BeginPlay()
{
    Super::BeginPlay();
    GenerateGrid();
}

void AGridManager::GenerateGrid()
{
    GridNodes.SetNum(GridSizeX);
    for (int32 X = 0; X < GridSizeX; X++)
    {
        GridNodes[X].SetNum(GridSizeY);
        for (int32 Y = 0; Y < GridSizeY; Y++)
        {
            FVector WorldPos = GetActorLocation() + FVector(X * NodeSize, Y * NodeSize, 0);

            // Check for obstacles
            bool bObstacle = GetWorld()->OverlapBlockingTestByChannel(
                WorldPos + FVector(0, 0, 50.0f), // Slightly above ground
                FQuat::Identity,
                ECC_WorldStatic,
                FCollisionShape::MakeSphere(NodeSize * 0.4f)
            );

            GridNodes[X][Y].WorldPosition = WorldPos;
            GridNodes[X][Y].bWalkable = !bObstacle; // Mark as unwalkable if an obstacle is found
            GridNodes[X][Y].GridX = X;
            GridNodes[X][Y].GridY = Y;
        }
    }
}


FGridNode* AGridManager::GetNodeFromWorldPosition(FVector WorldPosition)
{
    int32 X = FMath::Clamp(FMath::RoundToInt((WorldPosition.X - GetActorLocation().X) / NodeSize), 0, GridSizeX - 1);
    int32 Y = FMath::Clamp(FMath::RoundToInt((WorldPosition.Y - GetActorLocation().Y) / NodeSize), 0, GridSizeY - 1);
    return &GridNodes[X][Y];
}
TArray<FVector> AGridManager::FindPath(FVector StartWorldPos, FVector TargetWorldPos)
{
    TArray<FVector> Path;

    FGridNode* StartNode = GetNodeFromWorldPosition(StartWorldPos);
    FGridNode* TargetNode = GetNodeFromWorldPosition(TargetWorldPos);

    if (!StartNode || !TargetNode)
    {
        return Path; // Return empty path if nodes are invalid
    }

    TArray<FGridNode*> OpenSet;
    TSet<FGridNode*> ClosedSet;

    OpenSet.Add(StartNode);

    while (OpenSet.Num() > 0)
    {
        FGridNode* CurrentNode = OpenSet[0];
        for (int32 i = 1; i < OpenSet.Num(); i++)
        {
            if (OpenSet[i]->FCost() < CurrentNode->FCost() ||
                (OpenSet[i]->FCost() == CurrentNode->FCost() && OpenSet[i]->HCost < CurrentNode->HCost))
            {
                CurrentNode = OpenSet[i];
            }
        }

        OpenSet.Remove(CurrentNode);
        ClosedSet.Add(CurrentNode);

        if (CurrentNode == TargetNode)
        {
            Path = RetracePath(StartNode, TargetNode);
            return Path;
        }

        for (FGridNode* Neighbor : GetNeighbors(CurrentNode))
        {
            if (ClosedSet.Contains(Neighbor) || !Neighbor->bWalkable) // Ignore non-walkable nodes
            {
                continue;
            }

            int32 NewMovementCost = CurrentNode->GCost + GetDistance(CurrentNode, Neighbor);
            if (NewMovementCost < Neighbor->GCost || !OpenSet.Contains(Neighbor))
            {
                Neighbor->GCost = NewMovementCost;
                Neighbor->HCost = GetDistance(Neighbor, TargetNode);
                Neighbor->Parent = CurrentNode;

                if (!OpenSet.Contains(Neighbor))
                {
                    OpenSet.Add(Neighbor);
                }
            }
        }
    }

    return Path; // No valid path found
}

TArray<FVector> AGridManager::RetracePath(FGridNode* StartNode, FGridNode* EndNode)
{
    TArray<FVector> Path;
    FGridNode* CurrentNode = EndNode;

    while (CurrentNode != StartNode)
    {
        Path.Add(CurrentNode->WorldPosition);
        CurrentNode = CurrentNode->Parent;
    }

    Algo::Reverse(Path);
    return Path;
}
TArray<FGridNode*> AGridManager::GetNeighbors(FGridNode* Node)
{
    TArray<FGridNode*> Neighbors;
    for (int32 X = -1; X <= 1; X++)
    {
        for (int32 Y = -1; Y <= 1; Y++)
        {
            if (X == 0 && Y == 0) continue;
            int32 CheckX = Node->GridX + X;
            int32 CheckY = Node->GridY + Y;
            if (CheckX >= 0 && CheckX < GridSizeX && CheckY >= 0 && CheckY < GridSizeY)
            {
                Neighbors.Add(&GridNodes[CheckX][CheckY]); // Fixed pointer
            }
        }
    }
    return Neighbors;
}
int32 AGridManager::GetDistance(FGridNode* NodeA, FGridNode* NodeB)
{
    int32 DistX = FMath::Abs(NodeA->GridX - NodeB->GridX);
    int32 DistY = FMath::Abs(NodeA->GridY - NodeB->GridY);

    if (DistX > DistY)
        return 14 * DistY + 10 * (DistX - DistY);

    return 14 * DistX + 10 * (DistY - DistX);
}
void AGridManager::DrawGrid()
{
    for (int32 X = 0; X < GridSizeX; X++)
    {
        for (int32 Y = 0; Y < GridSizeY; Y++)
        {
            FGridNode& Node = GridNodes[X][Y];

            // Draw node position
            FColor NodeColor = Node.bWalkable ? FColor::Green : FColor::Red; // Red for obstacles
            DrawDebugPoint(GetWorld(), Node.WorldPosition, 10.0f, NodeColor, false, 0.1f); // Draw node as point

            // Draw grid lines to represent the grid structure
            FVector Right = Node.WorldPosition + FVector(NodeSize, 0, 0); // Right direction for line
            FVector Up = Node.WorldPosition + FVector(0, NodeSize, 0);    // Up direction for line

            DrawDebugLine(GetWorld(), Node.WorldPosition, Right, FColor::White, false, 0.1f); // Horizontal line
            DrawDebugLine(GetWorld(), Node.WorldPosition, Up, FColor::White, false, 0.1f);    // Vertical line

        }
    }
}