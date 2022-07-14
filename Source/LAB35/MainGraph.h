// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>

#include "CoreMinimal.h"
#include "FloatingActor.h"
#include "GameFramework/Actor.h"
#include "MainGraph.generated.h"

UCLASS()
class LAB35_API AMainGraph : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMainGraph();

	ULineBatchComponent* LineBatch;
	// Отступ по X
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph")
	float OffsetX;

	// Отступ по Y
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph")
	float OffsetY;

	// Отступ по Z
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph")
	float OffsetZ;

	// Максимальное количество точек по X
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph")
	int GraphPointsCountX;

	// Максимальное количество точек по Y
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph")
	int GraphPointsCountY;
	
	// Максимальное количество шариков на сцене
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph")
	int BallCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ActorToMoveSettings")
	float ActorSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SplineController");
	UStaticMesh *Mesh;

	UPROPERTY()
	UInstancedStaticMeshComponent *MeshBetweenActors;

	UPROPERTY()
	UInstancedStaticMeshComponent *VertexMesh;
	
	// массив точек, заданных в формате FVector
	std::vector<FVector> Points;
	// массив ребер
	std::vector< std::vector<int> > Ribs;
	// матрица смежности
	std::vector< std::vector<double> > Matrix;

	std::vector<AFloatingActor*> FloatingActors;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;
	
	double Random(const double Min, const double Max);

	// для генерации точек и ребер графа
	void NewConjectureMatrix();

	void InitializeConjectureMatrix();

	void FillConjectureMatrix();

	std::vector<int> GetShortPath(int Start, int End);

	void CreateActorsToMove();

	void AddStaticMeshes();

	void SetMeshBetweenActors(FVector Start, FVector End);

	void SetVertex(FVector Position);

	FLinearColor GenerateActorColor();
	
};
