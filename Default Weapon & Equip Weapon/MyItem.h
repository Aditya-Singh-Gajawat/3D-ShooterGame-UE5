// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyItem.generated.h"


UENUM(BlueprintType)
enum class EStateOfItem :uint8
{
	ESOI_NotEquipped UMETA(DisplayName = "Not Equipped"),
	ESOI_IsToBeEquipped UMETA(DisplayName = "Is To Be Equipped"),
	ESOI_PickedUp UMETA(DisplayName = "PickedUp"),
	ESOI_Equipped UMETA(DisplayName = "Equipped"),
	ESOI_Falling UMETA(DisplayName = "Falling")
};

UCLASS()


class UE5POINT5_SHOOTER_API AMyItem : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* ItemSkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* ItemBoxCollider;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* WeaponWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* SphereDetector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	EStateOfItem StateOfItem;

public:	
	AMyItem();
	virtual void Tick(float DeltaTime) override;
	FORCEINLINE UWidgetComponent* ReturnWeaponWidget() const { return WeaponWidget; }
	FORCEINLINE UBoxComponent* ReturnItemBoxCollider() const { return ItemBoxCollider; }
	FORCEINLINE USphereComponent* ReturnSphereDetector() const { return SphereDetector; }
	FORCEINLINE EStateOfItem GetStateOfItem() const { return StateOfItem; }
	void SetStateOfItem(EStateOfItem State);

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	
	UFUNCTION()
	void OnSphereEndOverlap
	(
		UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	void SetItemProperties(EStateOfItem State);
};
