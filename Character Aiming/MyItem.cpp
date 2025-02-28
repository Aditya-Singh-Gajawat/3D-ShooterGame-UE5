#include "MyItem.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"

AMyItem::AMyItem()
{
	PrimaryActorTick.bCanEverTick = true;
	ItemStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item-Mesh"));
	SetRootComponent(ItemStaticMesh);
	
	ItemBoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	ItemBoxCollider->SetupAttachment(GetRootComponent());
	ItemBoxCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemBoxCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	
	WeaponWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Weapon-Widget"));
	WeaponWidget->SetupAttachment(GetRootComponent());
}

void AMyItem::BeginPlay()
{
	Super::BeginPlay();
	WeaponWidget->SetVisibility(false);
}

void AMyItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

