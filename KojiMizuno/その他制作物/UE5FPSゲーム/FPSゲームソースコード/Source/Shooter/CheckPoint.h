// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CheckPoint.generated.h"

UCLASS()
class SHOOTER_API ACheckPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Called when overlapping AreaSphere */
	UFUNCTION()
		void OnSphereOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFormSweep,
			const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	/** Static Mesh for the CheckPoint */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CheckPoint Properties", meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* CheckPointMesh;

	/** Collide to set location this actor */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CheckPoint Properties", meta = (AllowPrivateAccess = "true"))
		class USphereComponent* AreaSphere;

	/** Particle system spawned at the Overlap */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CheckPoint Properties", meta = (AllowPrivateAccess = "true"))
		class UParticleSystem* CheckPointParticle;

	/** Sound played when AreaSphere Overlapped */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CheckPoint Properties", meta = (AllowPrivateAccess = "true"))
		class USoundCue* CheckPointSound;
};
