#include "BasePickUpItem.h"
#include "MonsterShooterCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

// Sets default values
ABasePickUpItem::ABasePickUpItem()
{
	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(50.0f);
	RootComponent = CollisionComp;  // Set the Root Component for this actor

	// Enable overlap events
	CollisionComp->SetGenerateOverlapEvents(true);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABasePickUpItem::OnBeginOverlap);
	CollisionComp->OnComponentEndOverlap.AddDynamic(this, &ABasePickUpItem::OnEndOverlap);
}

// Called when the game starts or when spawned
void ABasePickUpItem::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABasePickUpItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// No need to check input here anymore
}

// Called when something begins to overlap
void ABasePickUpItem::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	AMonsterShooterCharacter* Character = Cast<AMonsterShooterCharacter>(OtherActor);
	if (Character)
	{
		bIsPlayerInRange = true;  // Player is in range
	}
}

// Called when something ends overlapping
void ABasePickUpItem::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMonsterShooterCharacter* Character = Cast<AMonsterShooterCharacter>(OtherActor);
	if (Character)
	{
		bIsPlayerInRange = false;  // Player is no longer in range
	}
}

void ABasePickUpItem::InteractWithPlayer(AMonsterShooterCharacter* ShooterCharacter)
{
	if (PickUpSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PickUpSound, ShooterCharacter->GetActorLocation());
	}

	// Destroy the pickup item after interaction
	Destroy();
}
