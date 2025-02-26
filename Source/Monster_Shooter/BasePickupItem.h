#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "BasePickUpItem.generated.h"

// Forward declarations
class AMonsterShooterCharacter;
class USoundBase;

UCLASS()
class MONSTER_SHOOTER_API ABasePickUpItem : public AActor
{
	GENERATED_BODY()

		/** Sphere collision component */
		UPROPERTY(VisibleDefaultsOnly)
		USphereComponent* CollisionComp;

public:
	// Sets default values for this actor's properties
	ABasePickUpItem();
	// Interaction function
	virtual void InteractWithPlayer(AMonsterShooterCharacter* ShooterCharacter);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
		USoundBase* PickUpSound;

	// Boolean flag to check if the player is in range
	bool bIsPlayerInRange = false;

private:
	/** Code for when something overlaps this component */
	UFUNCTION()
		void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
