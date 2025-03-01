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
	void AimingPressed();
	void AimingReleased();
	void CameraInterp(float DeltaTime);
	void SpawnFX(FName SocketName, UParticleSystem* ParticleFX);
	void SpawnBeamFX(FVector Start, FVector End);
	void PlayAnimation(UAnimMontage* AnimationMontage, FName SectionName);
	void PlaySound(USoundBase* SoundCue);
	bool GetBeamEndPointLocation(const FVector& SocketLocation, FVector& BeamEndLocation);
	bool TraceFromCrosshair(FHitResult& HitResult, FVector& HitLocation);
	void TraceItems();
	class AMyWeapon* DefaultWeaponSpawn();
	void EquipWeapon(AMyWeapon* WeaponToEquip);

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


	//Camera Zoom Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	FVector TargetCamLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	FRotator TargetCamRotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	bool bIsAiming;

	bool bTraceForHit;
	int8 IncrementValueForItemCount;
	class AMyItem* MyItemLastFrame;

	//Weapon Related Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	AMyWeapon* EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AMyWeapon> BaseWeaponClass; //TSubclassOf<AWeapon> allows you to use the AWeapon class or any of its child classes in Blueprints.

	FTimerHandle UltimateHandle; //Used for delaying ultimate
	FTimerHandle UltimateEmitterHandle;
	FTimerHandle PlayerInputTimeHandle;
public:

	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool ReturnIsAiming() const { return bIsAiming; }
	void IncrementOverlappedItemCount(int8 Value);
};
