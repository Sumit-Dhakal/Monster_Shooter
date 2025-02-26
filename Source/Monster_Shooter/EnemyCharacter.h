

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class MONSTER_SHOOTER_API AEnemyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    AEnemyCharacter();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    // Enemy health
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
        float Health;

    // Enemy max health
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
        float MaxHealth;
 




public:
    // Attack animation montage
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
        UAnimMontage* AttackMontage; // Reference to the attack animation montage

        // Attack animation duration
    UPROPERTY(EditDefaultsOnly, Category = "Combat")
        float AttackAnimationDuration = 1.0f; // Attack duration, adjust as needed

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
        USoundBase* AttackSound;  // This will hold the sound to play when the enemy attacks
    void PlayAttackSound();


        // Timer handle for attack animation reset
    FTimerHandle AttackAnimationTimerHandle;

    UPROPERTY(EditAnywhere, Category = "Movement")
        float RotationRate = 540.0f; // Degrees per second

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
        float DamageAmount;
    

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // Function to handle damage
    void DealDamage(float DamageAmount);

    // Function to check if the enemy is alive
    bool IsAlive() const;

    // Function to trigger the death of the enemy
    void Die();

    // Get the collision sphere used to detect the player
    class USphereComponent* GetDetectPlayerCollisionSphere();

    // Handles the attack animation logic
    void PlayAttackAnimation();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
        UAnimMontage* ScreamMontage; // Scream animation montage

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
        USoundBase* ScreamSound; // Scream sound effect
    // Function to play scream animation and notify when it's done
    UFUNCTION()
        void PlayScreamAndNotifyChase();
    // Add this to your class definition inside the public or protected section
    UPROPERTY(BlueprintReadWrite, Category = "AI")
        bool bHasPlayedScreamAnimation;  // Flag to track if the scream animation has been played
// Function to notify when the scream animation has finished
    UFUNCTION()
        void OnScreamAnimationFinished();
    bool bIsMovementBlocked = false;
    // Add a timer handle for scream animation cooldown
    FTimerHandle ScreamAnimationTimerHandle;
    void PlayScreamAnimation();

private:
    // Sphere component for detecting the player
    UPROPERTY(EditAnywhere, Category = "AI")
        class USphereComponent* DetectPlayerCollisionSphere;

    // Overlap event handlers
    UFUNCTION()
        void OnPlayerEnterDetectionRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
        void OnPlayerExitDetectionRange(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
