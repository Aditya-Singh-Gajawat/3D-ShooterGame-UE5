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


AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;


	AutoPossessPlayer = EAutoReceiveInput::Player0;

	//Ultimate Variables
	UltimateForceMagnitude = 500.0f;
	UltimateUpwardForce = 200.0f;
	// Set gravity scale
	GetCharacterMovement()->GravityScale = 10.0f;
	// Set character mass (if needed)
	GetCharacterMovement()->Mass = 100.0f;

	BaseTurnRate = 55.f;
	BaseLookUpRate = 55.f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.f; // The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//Character don't rotate when camera does
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	//Configure character movement further
	GetCharacterMovement()->bOrientRotationToMovement = true; //Character moves in  direction of input
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
	const USkeletalMeshSocket* Socket = GetMesh()->GetSocketByName(SocketName); // Retrieve the socket named by SocketName from the skeletal mesh
	if (Socket) // Check if the Socket is valid (not null)
	{
		const FTransform SocketTransform = Socket->GetSocketTransform(GetMesh()); // Get the transform (location, rotation, scale) of the socket
		if (ParticleFX) // Check if the particle system (ParticleFX) is valid (not null)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleFX, SocketTransform); // Spawn the particle emitter at the location of the socket's transform
		}
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

void AMyCharacter::EnablePlayerInput()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->EnableInput(PlayerController);
		//UE_LOG(LogTemp, Warning, TEXT("Input Enabled"));
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
}

