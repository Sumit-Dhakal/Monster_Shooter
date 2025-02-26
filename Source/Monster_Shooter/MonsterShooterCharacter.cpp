
#include "MonsterShooterCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Projectile.h"
#include "Blueprint/UserWidget.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "BasePickUpItem.h"  // Include the header for ABasePickUpItem
#include "MonsterShooterGameMode.h"
#include "EnemyCharacter.h"       // Add this
#include "Components/SphereComponent.h" // Add this


// Sets default values
AMonsterShooterCharacter::AMonsterShooterCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->InitCapsuleSize(40.0f, 95.0f);

	TurnRate = 45.0f;
	LookUpRate = 45.0f;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->AddRelativeLocation(FVector(-39.65f, 1.75f, 64.0f));
	FirstPersonCamera->bUsePawnControlRotation = true;

	HandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Character Mesh"));

	HandMesh->SetOnlyOwnerSee(true);
	HandMesh->SetupAttachment(FirstPersonCamera);
	HandMesh->bCastDynamicShadow = false;
	HandMesh->CastShadow = false;
	HandMesh->AddRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	HandMesh->AddRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun"));
	GunMesh->SetOnlyOwnerSee(true);
	GunMesh->bCastDynamicShadow = false;
	GunMesh->CastShadow = false;

	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle Location"));
	MuzzleLocation->SetupAttachment(GunMesh);
	MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;






}

// Called when the game starts or when spawned
void AMonsterShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	GunMesh->AttachToComponent(HandMesh,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		TEXT("GripPoint"));
	World = GetWorld();
	AnimInstance = HandMesh->GetAnimInstance();

}

// Called every frame
void AMonsterShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMonsterShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMonsterShooterCharacter::OnFire);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMonsterShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMonsterShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AMonsterShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AMonsterShooterCharacter::LookAtRate);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMonsterShooterCharacter::OnInteract);
}

float AMonsterShooterCharacter::GetHealth() const
{
	return CurrentHealth;
}

void AMonsterShooterCharacter::OnFire()
{
	if (World != nullptr)
	{
		SpawnRotation = GetControlRotation();
		SpawnLocation = ((MuzzleLocation != nullptr) ?
			MuzzleLocation->GetComponentLocation() :
			GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		World->SpawnActor<AProjectile>(Projectile, SpawnLocation, SpawnRotation, ActorSpawnParams);

		// Play sound
		if (FireSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}

		// Play animation
		if (FireAnimation != nullptr && AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.0f);
		}
	}
}

void AMonsterShooterCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
		PlayFootstepSound();
	}

}

void AMonsterShooterCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Value);
		PlayFootstepSound();
	}
}

void AMonsterShooterCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * TurnRate * GetWorld()->GetDeltaSeconds());
}

void AMonsterShooterCharacter::LookAtRate(float Rate)
{
	AddControllerPitchInput(Rate * LookUpRate * GetWorld()->GetDeltaSeconds());

}

void AMonsterShooterCharacter::DealDamage(float DamageAmount)
{
	CurrentHealth -= DamageAmount; 
	if (CurrentHealth <= 0.0f)
	{
		Die();
	}
}


void AMonsterShooterCharacter::Die()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, AEnemyCharacter::StaticClass());
	for (AActor* Actor : OverlappingActors)
	{
		AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(Actor);
		if (Enemy)
		{
			USphereComponent* Sphere = Enemy->GetDetectPlayerCollisionSphere();
			if (Sphere)
			{
				Sphere->OnComponentBeginOverlap.RemoveAll(Enemy);
				Sphere->OnComponentEndOverlap.RemoveAll(Enemy);
			}
		}
	}

	if (GameOverWidgetClass)
	{
		UUserWidget* GameOverWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
		if (GameOverWidget)
		{
			GameOverWidget->AddToViewport();
			UE_LOG(LogTemp, Warning, TEXT("Game Over UI Displayed"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create Game Over UI"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameOverWidgetClass is NULL"));
	}
}


void AMonsterShooterCharacter::PlayFootstepSound()
{
	if (FootstepSound && World)
	{
		static float LastFootstepTime = 0.0f;
		float CurrentTime = World->GetTimeSeconds();


		if (CurrentTime - LastFootstepTime > 0.4f)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FootstepSound, GetActorLocation());
			LastFootstepTime = CurrentTime;
		}
	}
}



void AMonsterShooterCharacter::OnInteract()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, ABasePickUpItem::StaticClass());

	for (AActor* Actor : OverlappingActors)
	{
		ABasePickUpItem* PickupItem = Cast<ABasePickUpItem>(Actor);
		if (PickupItem)
		{
			PickupItem->InteractWithPlayer(this);  // Interact with the pickup item
			PickupItem->Destroy();  // Optional: Destroy item after interaction
		}
	}
}

