#include "MyCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Particles/ParticleSystem.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Particles/ParticleSystemComponent.h"
#include "MyItem.h"
#include "MyWeapon.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"


AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bIsAiming = false;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	ZoomInterpSpeed = 5.f;

	BaseTurnRate = 55.f;
	BaseLookUpRate = 55.f;

	IncrementValueForItemCount = 0;
	bTraceForHit = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.f; // The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 50.f);
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//Character don't rotate when camera does
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true; //Set false, if want to control ultimate with keys
	//Configure character movement further
	GetCharacterMovement()->bOrientRotationToMovement = false; //Character moves in  direction of input
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // at this rotation
}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	EquipWeapon(DefaultWeaponSpawn());
}

void AMyCharacter::MoveForward(float Value)
{

	if((Controller!=nullptr) && (Value!=0.0f))
	{
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
		AddMovementInput(Direction, Value);
	}
}
void AMyCharacter::MoveRight(float Value)
{

	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
		AddMovementInput(Direction, Value);
	}
}
void AMyCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}
void AMyCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


void AMyCharacter::SpawnFX(FName SocketName, UParticleSystem* ParticleFX)
{
	
	if(!GetWorld())
	{
		return;
	}

	const USkeletalMeshSocket* Socket = GetMesh()->GetSocketByName(SocketName); // Retrieve the socket named by SocketName from the skeletal mesh
	if (Socket) // Check if the Socket is valid (not null)
	{
		const FTransform SocketTransform = Socket->GetSocketTransform(GetMesh()); // Get the transform (location, rotation, scale) of the socket
		if (ParticleFX) // Check if the particle system (ParticleFX) is valid (not null)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleFX, SocketTransform); // Spawn the particle emitter at the location of the socket's transform
		}

		FVector BeamEndPoint;

		bool bBeamEndPoint = GetBeamEndPointLocation(SocketTransform.GetLocation(), BeamEndPoint);

		if (bBeamEndPoint)
		{
			if (PistolHitFX)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PistolHitFX, BeamEndPoint);
			}

			FRotator BeamRotation = (BeamEndPoint - SocketTransform.GetLocation()).Rotation(); //Set orientation of the Beam FX
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PistolBeamFX, SocketTransform.GetLocation(), BeamRotation);
			//if (Beam)
			//{
			//	Beam->SetVectorParameter(FName("BeamSource"), SocketTransform.GetLocation()); // Start at muzzle
			//	Beam->SetVectorParameter(FName("BeamTarget"), BeamEndPoint); // End at hit location
			//} 
		}
	}
}
bool AMyCharacter::GetBeamEndPointLocation(const FVector& SocketLocation, FVector& BeamEndLocation)
{

	//Trace from Weapon Barrel

	FHitResult CrosshairHitResult;
	bool bCrosshairHit = TraceFromCrosshair(CrosshairHitResult, BeamEndLocation);
	
	if(bCrosshairHit)
	{
		BeamEndLocation = CrosshairHitResult.Location; //Storing location of hit under crosshair.
	}
	else
	{
		//BeamEndLocation is the End location for the line trace. Set in TraceForWidget().
	}

	FHitResult BarrelHitResult;
	const FVector WeaponTraceStart = SocketLocation;
	const FVector VectorFromStartToEnd = BeamEndLocation - SocketLocation;
	const FVector WeaponTraceEnd = SocketLocation + VectorFromStartToEnd * 2.5f; //Either no hit or hit from crosshair to object in the world

	GetWorld()->LineTraceSingleByChannel(BarrelHitResult, WeaponTraceStart, WeaponTraceEnd, ECollisionChannel::ECC_Visibility);

	DrawDebugLine(GetWorld(), WeaponTraceStart, WeaponTraceEnd, FColor::Red, false, 2.0f, 0, 1.5f);


	if (BarrelHitResult.bBlockingHit)
	{
		BeamEndLocation = BarrelHitResult.Location;
		if(BarrelHitResult.GetActor())
		{
			UE_LOG(LogTemp, Warning, TEXT("Beam Hit: %s at %s"), *BarrelHitResult.GetActor()->GetName(), *BarrelHitResult.Location.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Beam hit at location: %s, but no actor found."),
				*BarrelHitResult.Location.ToString());
		}
		return true;
	
	}
	return false;
}

bool AMyCharacter::TraceFromCrosshair(FHitResult& HitResult, FVector& HitLocation)
{
	if (!GEngine || !GEngine->GameViewport)
	{
		return false;
	}

	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	if (!bScreenToWorld)
		return false;

	const FVector Start = CrosshairWorldPosition;
	const FVector End = Start + CrosshairWorldDirection * 50'000.f;

	HitLocation = End; // Default to End if no hit occurs

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); // Ignore self

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECollisionChannel::ECC_Camera,
		QueryParams
	);

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.0f, 0, 1.5f);

	if (bHit && HitResult.bBlockingHit)
	{
		HitLocation = HitResult.Location;

		// Spawn the beam effect
		//SpawnBeamFX(Start, HitLocation);

		return true;
	}

	return false;
}

void AMyCharacter::SpawnBeamFX(FVector Start, FVector End)
{
	if (!PistolBeamFX) return; // Ensure the effect exists

	UParticleSystemComponent* BeamComponent = UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		PistolBeamFX,    // Your beam particle system
		Start      // Spawn at player position
	);
}

void AMyCharacter::TraceItems()
{
	if (bTraceForHit)
	{
		FHitResult WidgetHitResult;
		FVector DummyHitLocation; //Dummy Variable
		TraceFromCrosshair(WidgetHitResult, DummyHitLocation);
		if (WidgetHitResult.bBlockingHit)
		{
			AMyItem* Item = Cast<AMyItem>(WidgetHitResult.GetActor());
			if (Item && Item->ReturnWeaponWidget())
			{
				Item->ReturnWeaponWidget()->SetVisibility(true);
			}
			if(MyItemLastFrame)
			{
				if(Item!=MyItemLastFrame)
				{
					//We are hitting a different AItem this frame from last frame
					//OR
					//AItem is null
					MyItemLastFrame->ReturnWeaponWidget()->SetVisibility(false);
				}
			}
			MyItemLastFrame = Item; //Store a reference to Item for next frame
		}
	}
	else if(MyItemLastFrame)
	{
		//No longer overlapping any items,
		//Item last frame should not show widget
		MyItemLastFrame->ReturnWeaponWidget()->SetVisibility(false);
	}
}


void AMyCharacter::PlayAnimation(UAnimMontage* AnimationMontage, FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimationMontage && AnimInstance)
	{
		AnimInstance->Montage_Play(AnimationMontage);
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AMyCharacter::PlaySound(USoundBase* SoundCue)
{
	UGameplayStatics::PlaySound2D(this, SoundCue);
}

void AMyCharacter::FirePistol() // Functionality for firing pistol
{
	SpawnFX("gunMuzzleSocket", PistolMuzzleFX);
	PlayAnimation(PistolFireMontage, "Fire");
	PlaySound(PistolSoundCue);
}

void AMyCharacter::AimingPressed()
{
	bIsAiming = true;
}

void AMyCharacter::AimingReleased()
{
	bIsAiming = false;
}

void AMyCharacter::CameraInterp(float DeltaTime)
{
	TargetCamLocation = bIsAiming ? FVector(180.f, 0.f, 40.f) : FVector(0.f, 0.f, 0.f); //If true sets FVector(250.f, 0.f, -50.f), if false sets FVector(0.f, 0.f, 0.f)
	TargetCamRotation = bIsAiming ? FRotator(0.f, 0.f, 0.f) : FRotator(0.f, 0.f, 0.f);
	float TargetFOV = bIsAiming ? 75.f : 90.f;

	FVector NewCamLocation = FMath::VInterpTo(FollowCamera->GetRelativeLocation(), TargetCamLocation, DeltaTime, ZoomInterpSpeed);
	FRotator NewCamRotation = FMath::RInterpTo(FollowCamera->GetRelativeRotation(), TargetCamRotation, DeltaTime, ZoomInterpSpeed);
	float NewFOV = FMath::FInterpTo(FollowCamera->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);

	FollowCamera->SetRelativeLocation(NewCamLocation);
	FollowCamera->SetRelativeRotation(NewCamRotation);
	FollowCamera->FieldOfView = NewFOV;

}


AMyWeapon* AMyCharacter::DefaultWeaponSpawn()
{
	if(BaseWeaponClass) //Checking if TSubclassOf variable is valid, if true then
	{
		return GetWorld()->SpawnActor<AMyWeapon>(BaseWeaponClass); //Spawning default weapon into the world
	}
	return nullptr;
}

void AMyCharacter::EquipWeapon(AMyWeapon* WeaponToEquip)
{
	if (!WeaponToEquip) return;

	FName HandSocketName = FName("hand_rSocket");
	WeaponToEquip->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, HandSocketName);

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetStateOfItem(EStateOfItem::ESOI_Equipped);

	WeaponToEquip->ReturnItemBoxCollider()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	WeaponToEquip->ReturnSphereDetector()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CameraInterp(DeltaTime);
	TraceItems();
}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent); //Checks if PlayerInputComponent is not null.

	PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);
	PlayerInputComponent->BindAxis("BaseTurn", this, &AMyCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("BaseLookUp", this, &AMyCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput); //Mouse Y Movement
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput); //Mouse X Movement
	PlayerInputComponent->BindAction("FirePistol", EInputEvent::IE_Pressed, this, &AMyCharacter::FirePistol);
	PlayerInputComponent->BindAction("Aiming", EInputEvent::IE_Pressed, this, &AMyCharacter::AimingPressed);
	PlayerInputComponent->BindAction("Aiming", EInputEvent::IE_Released, this, &AMyCharacter::AimingReleased);
}

void AMyCharacter::IncrementOverlappedItemCount(int8 Value)
{
	if (IncrementValueForItemCount + Value <= 0)
	{
		IncrementValueForItemCount = 0;
		bTraceForHit = false;
	}
	else
	{
		IncrementValueForItemCount += Value;
		bTraceForHit = true;
	}
}
