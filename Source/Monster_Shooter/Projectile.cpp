#include "Projectile.h"
#include "MonsterShooterGameMode.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "EnemyCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectile::AProjectile()
{
    // Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // Initialize sphere collision
    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
    CollisionSphere->InitSphereRadius(20.0f);
    CollisionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    RootComponent = CollisionSphere;

    // Initialize projectile movement
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
    ProjectileMovement->UpdatedComponent = CollisionSphere;
    ProjectileMovement->InitialSpeed = 3000.0f;
    ProjectileMovement->MaxSpeed = 3000.0f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = true;

    // Default lifespan of the projectile
    InitialLifeSpan = 3.0f;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
    Super::BeginPlay();

    // Bind the OnHit event to the collision sphere
    CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnHit);
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Handles when the projectile overlaps with another actor
void AProjectile::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != this)
    {
        AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(OtherActor);
        if (Enemy)
        {
            // Deal damage to the enemy
            Enemy->DealDamage(ProjectileDamage);

            // Check if all enemies are defeated
            TArray<AActor*> AllEnemies;
            UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyCharacter::StaticClass(), AllEnemies);

            bool bAllEnemiesDefeated = true;
            for (AActor* EnemyActor : AllEnemies)
            {
                AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(EnemyActor);
                if (EnemyCharacter && EnemyCharacter->IsAlive())
                {
                    bAllEnemiesDefeated = false;
                    break;
                }
            }

            if (bAllEnemiesDefeated)
            {
                // Get the GameMode and call TransitionToNextRoom
                AMonsterShooterGameMode* GameMode = Cast<AMonsterShooterGameMode>(GetWorld()->GetAuthGameMode());
                if (GameMode)
                {
                    GameMode->TransitionToNextLevel(); // Call the level transition in GameMode
                }
            }

            // Destroy the projectile after impact
            Destroy();
        }
    }
}
