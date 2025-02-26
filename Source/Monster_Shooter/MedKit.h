#pragma once

#include "CoreMinimal.h"
#include "BasePickUpItem.h" // Base class
#include "MedKit.generated.h"

class AMonsterShooterCharacter;
class UParticleSystem;

UCLASS()
class MONSTER_SHOOTER_API AMedKit : public ABasePickUpItem
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AMedKit();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // The amount by which the medkit heals the player
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MedKit")
        float HealAmount;

    // Heal the player
    virtual void InteractWithPlayer(AMonsterShooterCharacter* ShooterCharacter) override;
    /** Healing effect particle system */
    UPROPERTY(EditAnywhere, Category = "Effects")
        TSubclassOf<AActor> HealAuraBlueprint;

};