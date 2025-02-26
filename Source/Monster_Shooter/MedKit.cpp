#include "MedKit.h"
#include "MonsterShooterCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMedKit::AMedKit()
{
    // Default values for the medkit can be set here if needed

}

// Called when the game starts or when spawned
void AMedKit::BeginPlay()
{
    Super::BeginPlay();
}

// Heal the player when interacting
void AMedKit::InteractWithPlayer(AMonsterShooterCharacter* ShooterCharacter)
{
    if (ShooterCharacter)
    {
        // Assume GetHealth() returns the current health
        float CurrentHealth = ShooterCharacter->GetHealth();
        float MaxHealth = 100.f; // Maximum allowed health
        float ActualHeal = HealAmount;

        // Clamp the healing so health does not exceed MaxHealth
        if (CurrentHealth + HealAmount > MaxHealth)
        {
            ActualHeal = MaxHealth - CurrentHealth;
        }

        // Heal the player by applying negative damage
        ShooterCharacter->DealDamage(-ActualHeal);

        // Play pickup sound
        if (PickUpSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, PickUpSound, GetActorLocation());
        }

        // Spawn healing effect on the player
        if (HealAuraBlueprint)
        {
            FActorSpawnParameters SpawnParams;
            AActor* HealAura = GetWorld()->SpawnActor<AActor>(
                HealAuraBlueprint,
                ShooterCharacter->GetActorLocation(),
                FRotator::ZeroRotator,
                SpawnParams
                );
            if (HealAura)
            {
                HealAura->AttachToComponent(
                    ShooterCharacter->GetMesh(),
                    FAttachmentTransformRules::SnapToTargetIncludingScale
                );
            }
        }

        // Optionally destroy the medkit after interaction
        // Destroy();
    }
}
