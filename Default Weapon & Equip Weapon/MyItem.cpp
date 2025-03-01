#include "MyItem.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "MyCharacter.h"

AMyItem::AMyItem()
{
	PrimaryActorTick.bCanEverTick = true;

	StateOfItem = EStateOfItem::ESOI_NotEquipped;

	ItemSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Item-Mesh"));
	SetRootComponent(ItemSkeletalMesh);
	
	ItemBoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	ItemBoxCollider->SetupAttachment(GetRootComponent());
	ItemBoxCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemBoxCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	
	WeaponWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Weapon-Widget"));
	WeaponWidget->SetupAttachment(GetRootComponent());

	SphereDetector = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere-Detector"));
	SphereDetector->SetupAttachment(GetRootComponent());
}


void AMyItem::BeginPlay()
{
	Super::BeginPlay();
	WeaponWidget->SetVisibility(false);
	SphereDetector->OnComponentBeginOverlap.AddDynamic(this, &AMyItem::OnSphereOverlap);
	SphereDetector->OnComponentEndOverlap.AddDynamic(this, &AMyItem::OnSphereEndOverlap);
}

void AMyItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor)
	{
		AMyCharacter* MyCharacter = Cast<AMyCharacter>(OtherActor);
		if(MyCharacter)
		{
			MyCharacter->IncrementOverlappedItemCount(1);
		}
	}
}

void AMyItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMyCharacter* MyCharacter = Cast<AMyCharacter>(OtherActor);
		if (MyCharacter)
		{
			MyCharacter->IncrementOverlappedItemCount(-1);
		}
	}
}

void AMyItem::SetItemProperties(EStateOfItem State)
{
	switch (State)
	{
	case EStateOfItem::ESOI_NotEquipped:
		//Set Mesh Properties
		ItemSkeletalMesh->SetSimulatePhysics(false);
		ItemSkeletalMesh->SetVisibility(true);
		ItemSkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		//Set Area Sphere Properties
		SphereDetector->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		SphereDetector->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		//Set Collision Box Properties
		ItemBoxCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemBoxCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
		ItemBoxCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;

	case EStateOfItem::ESOI_Equipped:
		//Set Mesh Properties
		ItemSkeletalMesh->SetSimulatePhysics(false);
		ItemSkeletalMesh->SetVisibility(true);
		ItemSkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//Set Area Sphere Properties
		SphereDetector->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		SphereDetector->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//Set Collision Box Properties
		ItemBoxCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemBoxCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	default:
		break;
	}
}

void AMyItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AMyItem::SetStateOfItem(EStateOfItem State)
{
	StateOfItem = State;
	SetItemProperties(State);
}