#include "MyAnimInstance.h"
#include "MyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


void UMyAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (Drongo == nullptr)
		Drongo = Cast<AMyCharacter>(TryGetPawnOwner());
	
	if(Drongo)
	{
		FVector Velocity = Drongo->GetVelocity();
		Velocity.Z = 0.f;
		Speed = Velocity.Size(); //Stores Character's (Drongo) Speed
	
		if (Drongo->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f) 
			bIsAcclerating = true;
		else
			bIsAcclerating = false;
		
		FRotator PlayerAimRotation = Drongo->GetBaseAimRotation();
		FRotator PlayerMoveRotation = UKismetMathLibrary::MakeRotFromX(Drongo->GetVelocity());
		//Calculating strafe value
		StrafeValue = UKismetMathLibrary::NormalizedDeltaRotator(PlayerMoveRotation, PlayerAimRotation).Yaw;
		if (Drongo->GetVelocity().Size() > 0.f)
		{
			LastStrafeValue = StrafeValue; //Stores last strafe value if Velocity of character is not zero.
		}
		bIsAiming = Drongo->ReturnIsAiming();
	}
}

void UMyAnimInstance::NativeInitializeAnimation()
{
	//Get Pawn Owner
	Drongo = Cast<AMyCharacter>(TryGetPawnOwner()); //We cast AMyCharacter to Drongo, our player character.
}
