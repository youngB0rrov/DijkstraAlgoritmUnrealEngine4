// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingActor.generated.h"

UCLASS()
class LAB35_API AFloatingActor : public AActor
{
	GENERATED_BODY()
	
	// для того чтобы объект staticMesh был виден внутри движка
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent *VisualMesh;

public:	
	// Sets default values for this actor's properties
	AFloatingActor();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FloatingActor")
	float FloatSpeed = 20.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FloatingActor")
	float RotationSpeed = 10.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	int StartPoint, EndPoint;
	float ActorSpeed;
	FVector CurrentActorPosition, NextActorPosition;
	std::vector<int> ActorShortPath;
	bool ActorCanBeMoved;
	int CurrentVertex;
	std::vector<FVector> ActorPoints;
	std::vector< std::vector<double> > CopiedMatrix;
	ULineBatchComponent *LineBatch;
	int NumberOfActors;
	FLinearColor ActorPathColor;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetStartPoint(const int PointIndex);
	void SetEndPoint(const int PointIndex);
	void SetActorShortPath(const std::vector<int> ShortPath);
	void SetActorSpeed(const float Speed);
	void CreateActorPath();
	void SetPoints(std::vector<FVector> GraphPoints);
	void SetNumberOfActors(int ActorsNumber) { NumberOfActors = ActorsNumber; }
	void SetActorPathColor(FLinearColor Color) { this-> ActorPathColor = Color; }
	void CopyMatrix(std::vector< std::vector<double> > Matrix) { CopiedMatrix = Matrix; }
	std::vector<int> getShortPathWhenFinished(int Start, int End);
};


