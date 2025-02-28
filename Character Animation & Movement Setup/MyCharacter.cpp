#include "MyCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Particles/ParticleSystem.h"


AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

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

void AMyCharacter::FirePistol() //Functionality for firing pistol
{
	// Spawning Particle System - MuzzleFX
	const USkeletalMeshSocket* PistolMuzzleSocket = GetMesh()->GetSocketByName("pistolMuzzle"); // Retrieve the socket named "pistolMuzzle" from the skeletal mesh and store its information in PistolMuzzleSocket
	if (PistolMuzzleSocket) // Check if the PistolMuzzleSocket is valid (not null)
	{
		const FTransform SocketTransform = PistolMuzzleSocket->GetSocketTransform(GetMesh()); // Get the transform (location, rotation, scale) of the "pistolMuzzle" socket
		if (PistolMuzzleFX) // Check if the particle system (PistolMuzzleFX) is valid (not null)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PistolMuzzleFX, SocketTransform); // Spawn the particle emitter at the location of the socket's transform
		}
	}
	//Sound Cue
	UGameplayStatics::PlaySound2D(this, PistolSoundCue);
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
}

