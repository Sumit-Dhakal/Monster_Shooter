#include "EnemyCharacter.h"
#include "MonsterShooterCharacter.h"
#include "MonsterShooterGameMode.h"
#include "EnemyAIController.h"
#include "Animation/AnimMontage.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"  // This header is required to use UGameplayStatics
#include "Sound/SoundBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h" 
#include "Components/SphereComponent.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
    // Set this character to call Tick() every frame.
    PrimaryActorTick.bCanEverTick = true;

    // Initialize the collision sphere for detecting the player
    DetectPlayerCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));
    DetectPlayerCollisionSphere->SetupAttachment(RootComponent);

    // Set the detection radius
    DetectPlayerCollisionSphere->SetSphereRadius(1000.0f); // Adjust as needed
    DetectPlayerCollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    DetectPlayerCollisionSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    DetectPlayerCollisionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    DetectPlayerCollisionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

    // Initialize health
    MaxHealth = 100.0f; // Default max health
    Health = MaxHealth;

    // Add these lines at the end of the constructor
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, RotationRate, 0.0f);

        // Default damage amount
        DamageAmount = 20.0f; // Adjust as needed for the default enemy

}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Bind overlap events
    if (DetectPlayerCollisionSphere)
    {
        DetectPlayerCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemyCharacter::OnPlayerEnterDetectionRange);
        DetectPlayerCollisionSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemyCharacter::OnPlayerExitDetectionRange);
    }
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemyCharacter::DealDamage(float DamageToApply)
{
    if (DamageToApply <= 0.0f) return;

    Health -= DamageToApply; // Subtract the parameter value from the health

    if (Health <= 0.0f)
    {
        Die();
    }
}





// Function to trigger the death of the enemy
void AEnemyCharacter::Die()
{
    if (!IsValid(this)) return; // Early exit if already destroyed

    AMonsterShooterGameMode* GameMode = Cast<AMonsterShooterGameMode>(GetWorld()->GetAuthGameMode());
    if (IsValid(GameMode))
    {
        GameMode->IncrementKillCount();
    }
    Destroy();
}



// Get the collision sphere used to detect the player
USphereComponent* AEnemyCharacter::GetDetectPlayerCollisionSphere()
{
    return DetectPlayerCollisionSphere;
}
void AEnemyCharacter::PlayAttackSound()
{
    if (AttackSound)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), AttackSound, GetActorLocation());
    }
}

// Function to handle the attack animation
void AEnemyCharacter::PlayAttackAnimation()
{
    if (AttackMontage)
    {
        UE_LOG(LogTemp, Warning, TEXT("Attack Montage Found, Playing..."));
        PlayAnimMontage(AttackMontage);
        PlayAttackSound();  // Call the sound function
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No Attack Montage Found!"));
    }
}







// Overlap event when the player enters the detection range
void AEnemyCharacter::OnPlayerEnterDetectionRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // Ensure the overlapping actor is the player
    if (OtherActor && OtherActor->IsA(AMonsterShooterCharacter::StaticClass()))
    {
        // Player is detected, now trigger the attack logic
        if (!bHasPlayedScreamAnimation)
        {
            // Play the scream animation first
            PlayScreamAndNotifyChase();
        }
        else
        {
            // If scream animation has been played, check if the player is close enough to attack
            float DistanceToPlayer = FVector::Dist(GetActorLocation(), OtherActor->GetActorLocation());
            if (DistanceToPlayer <= 500.0f)  // Adjust this range as needed
            {
                PlayAttackAnimation();  // Play the attack animation only if in range
            }
            else
            {
                // Player is not in range, just start chasing
                if (AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController()))
                {
                    AIController->StartChasingPlayer();
                }
            }
        }
    }
}


// Overlap event when the player leaves the detection range
void AEnemyCharacter::OnPlayerExitDetectionRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    // Ensure the overlapping actor is the player
    if (OtherActor && OtherActor->IsA(AMonsterShooterCharacter::StaticClass()))
    {
        // Notify the AI Controller
        if (AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController()))
        {
            AIController->OnDetectPlayerEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
        }
    }
}

// Function to check if the enemy is alive
bool AEnemyCharacter::IsAlive() const
{
    return Health > 0.0f;
}


void AEnemyCharacter::PlayScreamAnimation()
{
    if (bHasPlayedScreamAnimation)
    {
        return;  // Skip if the scream animation has already been played
    }

    // Disable movement
    bIsMovementBlocked = true;
    GetCharacterMovement()->DisableMovement();

    if (ScreamMontage)
    {
        PlayAnimMontage(ScreamMontage);
    }

    if (ScreamSound)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), ScreamSound, GetActorLocation());
    }

    // Mark scream animation as played
    bHasPlayedScreamAnimation = true;

    // Set a timer to re-enable movement after the animation ends
    float AnimLength = ScreamMontage->GetPlayLength();
    GetWorld()->GetTimerManager().SetTimer(ScreamAnimationTimerHandle, this, &AEnemyCharacter::OnScreamAnimationFinished, AnimLength, false);
}


void AEnemyCharacter::PlayScreamAndNotifyChase()
{
    if (ScreamMontage)
    {
        // Play the scream animation
        PlayAnimMontage(ScreamMontage);
        bHasPlayedScreamAnimation = true;  // Mark that the scream animation has been played

        // Freeze the enemy's movement during the scream animation
        GetCharacterMovement()->DisableMovement();  // Disable movement

        // Get the player's location
        AActor* PlayerActor = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0); // Get the player (assuming single player)

        if (PlayerActor)
        {
            // Calculate the direction from the enemy to the player
            FVector PlayerLocation = PlayerActor->GetActorLocation();
            FVector DirectionToPlayer = PlayerLocation - GetActorLocation();
            DirectionToPlayer.Z = 0.0f;  // Ignore the Z-axis to keep the rotation level (ignore vertical movement)

            // Calculate the rotation to face the player
            FRotator NewRotation = FRotationMatrix::MakeFromX(DirectionToPlayer).Rotator();

            // Set the enemy's rotation to face the player
            SetActorRotation(NewRotation);
        }

        // Set a timer to notify AI to start chasing after the animation ends
        float AnimLength = ScreamMontage->GetPlayLength();
        GetWorld()->GetTimerManager().SetTimer(ScreamAnimationTimerHandle, this, &AEnemyCharacter::OnScreamAnimationFinished, AnimLength, false);
    }

    // Play scream sound at the enemy's location
    if (ScreamSound)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), ScreamSound, GetActorLocation());
    }
}


void AEnemyCharacter::OnScreamAnimationFinished()
{
    // Enable movement after the scream animation finishes
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

    // Start chasing player after scream animation finishes
    if (AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController()))
    {
        AIController->StartChasingPlayer();  // Now the enemy can chase the player
    }
}
