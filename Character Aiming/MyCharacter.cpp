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
#include "Components/WidgetComponent.h"


AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bIsAiming = false;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	//Ultimate Variables
	UltimateForceMagnitude = 500.0f;
	UltimateUpwardForce = 200.0f;
	// Set gravity scale
	GetCharacterMovement()->GravityScale = 10.0f;
	// Set character mass (if needed)
	GetCharacterMovement()->Mass = 100.f;

	ZoomInterpSpeed = 5.f;

	BaseTurnRate = 55.f;
	BaseLookUpRate = 55.f;

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

			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PistolBeamFX, SocketTransform);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEndPoint);
			}
		}
	}
}

//bool AMyCharacter::GetBeamEndPointLocation(const FVector& SocketLocation, FVector& BeamEndLocation)
//{
//	if (!GEngine && !GEngine->GameViewport)
//	{
//		return false; //Early return if GEngine or GameViewport is invalid
//	}
//
//	FVector2D ViewportSize;
//	GEngine->GameViewport->GetViewportSize(ViewportSize);
//	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
//	FVector CrosshairWorldPosition;
//	FVector CrosshairWorldDirection;
//	
//	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld
//	(
//		UGameplayStatics::GetPlayerController(this, 0),
//		CrosshairLocation,
//		CrosshairWorldPosition,
//		CrosshairWorldDirection
//	);
//
//	if (!bScreenToWorld) 
//	{
//		return false;
//	}
//
//	//Trace from Crosshair
//	FHitResult ScreenHitResult;
//	const FVector Start = CrosshairWorldPosition;
//	const FVector End = CrosshairWorldPosition + CrosshairWorldDirection * 50000.f;
//
//	BeamEndLocation = End; //No Hit
//
//	GetWorld()->LineTraceSingleByChannel(ScreenHitResult, Start, End, ECollisionChannel::ECC_Visibility);
//
//	if (ScreenHitResult.bBlockingHit)
//	{
//		BeamEndLocation = ScreenHitResult.Location; //Storing location of hit from crosshair to object in the world
//	}
//
//	//Trace from Weapon Barrel
//	FHitResult BarrelHitResult;
//	const FVector WeaponTraceStart = SocketLocation;
//	const FVector WeaponTraceEnd = BeamEndLocation; //Either no hit or hit from crosshair to object in the world
//
//	GetWorld()->LineTraceSingleByChannel(BarrelHitResult, WeaponTraceStart, WeaponTraceEnd, ECollisionChannel::ECC_Visibility);
//	if (BarrelHitResult.bBlockingHit)
//	{
//		BeamEndLocation = BarrelHitResult.Location;
//	}
//	return true;
//}

//bool AMyCharacter::TraceForWidget(FHitResult& HitResult)
//{
//	if (!GEngine && !GEngine->GameViewport)
//		return false;
//
//	FVector2D ViewportSize;
//	GEngine->GameViewport->GetViewportSize(ViewportSize);
//	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
//	FVector CrosshairWorldPosition;
//	FVector CrosshairWorldDirection;
//
//	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld
//	(
//		UGameplayStatics::GetPlayerController(this, 0),
//		CrosshairLocation,
//		CrosshairWorldPosition,
//		CrosshairWorldDirection
//	);
//
//	if (!bScreenToWorld)
//		return false;
//	
//	// Trace from Crosshair world location outward
//	const FVector Start{ CrosshairWorldPosition };
//	const FVector End{ Start + CrosshairWorldDirection * 50'000.f };
//	GetWorld()->LineTraceSingleByChannel(
//		HitResult,
//		Start,
//		End,
//		ECollisionChannel::ECC_Visibility);
//
//	if (HitResult.bBlockingHit)
//	{
//			return true;
//	}
//
//	return false;
//}

bool AMyCharacter::GetBeamEndPointLocation(const FVector& SocketLocation, FVector& BeamEndLocation)
{
	FHitResult CrosshairHitResult;
	bool bCrosshairHit = TraceForWidget(CrosshairHitResult, BeamEndLocation);
	
	if(bCrosshairHit)
	{
		BeamEndLocation = CrosshairHitResult.Location; //Storing location of hit under crosshair.
	}
	else
	{
		//BeamEndLocation is the End location for the line trace. Set in TraceForWidget().
	}

	//Trace from Weapon Barrel
	FHitResult BarrelHitResult;
	const FVector WeaponTraceStart = SocketLocation;
	const FVector VectorFromStartToEnd = BeamEndLocation - SocketLocation;
	const FVector WeaponTraceEnd = SocketLocation + VectorFromStartToEnd * 1.25f; //Either no hit or hit from crosshair to object in the world

	GetWorld()->LineTraceSingleByChannel(BarrelHitResult, WeaponTraceStart, WeaponTraceEnd, ECollisionChannel::ECC_Visibility);
	if (BarrelHitResult.bBlockingHit)
	{
		BeamEndLocation = BarrelHitResult.Location;
		return true;
	}
	return false;
}


bool AMyCharacter::TraceForWidget(FHitResult& HitResult, FVector& HitLocation)
{
	if(!GEngine && !GEngine->GameViewport)
	{
		return false;
	}
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld
	(
		UGameplayStatics::GetPlayerController(this,0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	if (!bScreenToWorld)
		return false;

	const FVector Start = CrosshairWorldPosition;
	const FVector End = Start + CrosshairWorldDirection * 50'000.f;

	HitLocation = End; //If no HitResult.bBlockingHit then we have HitLocation stores End

	GetWorld()->LineTraceSingleByChannel
	(
		HitResult,
		Start,
		End,
		ECollisionChannel::ECC_Visibility
	);

	if(HitResult.bBlockingHit)
	{
		HitLocation = HitResult.Location;
		return true;
	}

	return false;
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

void AMyCharacter::ApplyForceWhenUltimateIsUsed(float ForceMagnitude, float UpwardForce)
{
	FVector LaunchDirection = GetActorForwardVector();
	LaunchDirection.Normalize();

	float Mass = GetCharacterMovement()->Mass;

	float AdjustedForce = ForceMagnitude * GetCharacterMovement()->GravityScale * Mass;
	float AdjustedUpward = UpwardForce * GetCharacterMovement()->GravityScale * Mass;

	FVector Force = -LaunchDirection * ForceMagnitude + FVector(0.f,0.f,UpwardForce);

	GetCharacterMovement()->AddImpulse(Force, true);

	//UE_LOG(LogTemp, Warning, TEXT("Force applied to character: %s"), *Force.ToString());
}

void AMyCharacter::FirePistol() // Functionality for firing pistol
{
	SpawnFX("pistolMuzzle", PistolMuzzleFX);
	PlayAnimation(PistolFireMontage, "Fire");
	PlaySound(PistolSoundCue);
}

void AMyCharacter::UltimateFire()
{
	//Disable Player Inputs
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController) 
	{
		PlayerController->DisableInput(PlayerController);
		//UE_LOG(LogTemp, Warning, TEXT("Input Disabled"));
	}

	PlayAnimation(UltimateFireMontage, "Ultimate");
	float DelayForAnimationDuration = UltimateFireMontage->GetPlayLength();

	GetWorldTimerManager().SetTimer(PlayerInputTimeHandle, this, &AMyCharacter::EnablePlayerInput, DelayForAnimationDuration, false);
	GetWorldTimerManager().SetTimer(UltimateHandle, this, &AMyCharacter::DelayedUltimateAbility, UltimateAbilityDelay, false);
	GetWorldTimerManager().SetTimer(UltimateEmitterHandle, this, &AMyCharacter::DelayedUltimateAbilityEmitter, UltimateAbilityEmitterDelay, false);
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
	TargetCamLocation = bIsAiming ? FVector(250.f, 0.f, -50.f) : FVector(0.f, 0.f, 0.f); //If true sets FVector(250.f, 0.f, -50.f), if false sets FVector(0.f, 0.f, 0.f)
	TargetCamRotation = bIsAiming ? FRotator(0.f, 0.f, 0.f) : FRotator(0.f, 0.f, 0.f); 
	float TargetFOV = bIsAiming ? 75.f : 90.f;

	FVector NewCamLocation = FMath::VInterpTo(FollowCamera->GetRelativeLocation(), TargetCamLocation, DeltaTime, ZoomInterpSpeed);
	FRotator NewCamRotation = FMath::RInterpTo(FollowCamera->GetRelativeRotation(), TargetCamRotation, DeltaTime, ZoomInterpSpeed);
	float NewFOV = FMath::FInterpTo(FollowCamera->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);

	FollowCamera->SetRelativeLocation(NewCamLocation);
	FollowCamera->SetRelativeRotation(NewCamRotation);
	FollowCamera->FieldOfView = NewFOV;
}

void AMyCharacter::EnablePlayerInput()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->EnableInput(PlayerController);
	}
}


void AMyCharacter::DelayedUltimateAbility()
{
	ApplyForceWhenUltimateIsUsed(UltimateForceMagnitude, UltimateUpwardForce);
}

void AMyCharacter::DelayedUltimateAbilityEmitter()
{
	SpawnFX("bazookaMuzzle", UltimateMuzzleFX); // Functionality for firing ultimate ability
	PlaySound(UltimateSoundCue);
}


void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CameraInterp(DeltaTime);

	FHitResult WidgetHitResult;
	FVector DummyHitLocation; //Dummy Variable
	TraceForWidget(WidgetHitResult, DummyHitLocation);
	if(WidgetHitResult.bBlockingHit)
	{
		AMyItem* Item = Cast<AMyItem>(WidgetHitResult.GetActor());
		if (Item && Item->ReturnWeaponWidget())
			Item->ReturnWeaponWidget()->SetVisibility(true);
	}
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
	PlayerInputComponent->BindAction("UltimateAbility", EInputEvent::IE_Pressed, this, &AMyCharacter::UltimateFire);
	PlayerInputComponent->BindAction("Aiming", EInputEvent::IE_Pressed, this, &AMyCharacter::AimingPressed);
	PlayerInputComponent->BindAction("Aiming", EInputEvent::IE_Released, this, &AMyCharacter::AimingReleased);
}

