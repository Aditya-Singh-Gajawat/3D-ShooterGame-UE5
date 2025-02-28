// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyItem.generated.h"


UCLASS()
class UE5POINT5_SHOOTER_API AMyItem : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ItemStaticMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* ItemBoxCollider;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* WeaponWidget;

public:	
	AMyItem();
	virtual void Tick(float DeltaTime) override;
	FORCEINLINE UWidgetComponent* ReturnWeaponWidget() const { return WeaponWidget; }

protected:
	virtual void BeginPlay() override;
};
