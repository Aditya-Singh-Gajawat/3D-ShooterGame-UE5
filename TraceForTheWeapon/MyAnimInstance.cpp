#include "MyAnimInstance.h"
#include "MyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

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
	}
}

void UMyAnimInstance::NativeInitializeAnimation()
{
	//Get Pawn Owner
	Drongo = Cast<AMyCharacter>(TryGetPawnOwner()); //We cast AMyCharacter to Drongo, our player character.
}
