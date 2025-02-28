// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

UCLASS()

class UE5POINT5_SHOOTER_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	AMyCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


protected:

	virtual void BeginPlay() override;
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	void FirePistol();
	void UltimateFire();
	void AimingPressed();
	void AimingReleased();
	void CameraInterp(float DeltaTime);
	void SpawnFX(FName SocketName, UParticleSystem* ParticleFX);
	void PlayAnimation(UAnimMontage* AnimationMontage, FName SectionName);
	void PlaySound(USoundBase* SoundCue);
	void ApplyForceWhenUltimateIsUsed(float Total_Force, float Upward_Force);
	void DelayedUltimateAbility();
	void DelayedUltimateAbilityEmitter();
	void EnablePlayerInput();
	bool GetBeamEndPointLocation(const FVector& SocketLocation, FVector& BeamEndLocation);
	bool TraceForWidget(FHitResult& HitResult, FVector& HitLocation);

private:
	//Camera boom positioning the camera behind the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	//Camera that follows the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	//Base turn rate
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;
	//Base look up rate
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;
	
	//Sound Cue - Pistol
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat , meta = (AllowPrivateAccess = "true"))
	class USoundCue* PistolSoundCue;
	//Muzzle Pistol Flash
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* PistolMuzzleFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* PistolHitFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* PistolBeamFX;
	//Primary Fire Anim Montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* PistolFireMontage;

	//Sound Cue - Ultimate
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* UltimateSoundCue;
	//Muzzle Ultimate Flash
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* UltimateMuzzleFX;
	//Ultimate Ultimate Anim Montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* UltimateFireMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ultimate, meta = (AllowPrivateAccess = "true"))
	float UltimateForceMagnitude;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ultimate, meta = (AllowPrivateAccess = "true"))
	float UltimateUpwardForce;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ultimate, meta = (AllowPrivateAccess = "true"))
	float UltimateAbilityDelay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ultimate, meta = (AllowPrivateAccess = "true"))
	float UltimateAbilityEmitterDelay;
	//Camera Zoom Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	FVector TargetCamLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	FRotator TargetCamRotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	bool bIsAiming;

	FTimerHandle UltimateHandle; //Used for delaying ultimate
	FTimerHandle UltimateEmitterHandle;
	FTimerHandle PlayerInputTimeHandle;
public:

	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool ReturnIsAiming() const { return bIsAiming; }
};
