#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class MONSTER_SHOOTER_API AProjectile : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AProjectile();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Sphere collision for detecting hits
    UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
        class USphereComponent* CollisionSphere;

    // Handles projectile movement
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
        class UProjectileMovementComponent* ProjectileMovement;

    // Called when the projectile hits something
    UFUNCTION()
        void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    // Damage dealt by the projectile
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
        float ProjectileDamage = 20.0f; // Default damage value
};
