// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGraph.h"

#include "FloatingActor.h"
#include "Components/LineBatchComponent.h"
#include "Components/StaticMeshComponent.h"
#define MAX_LABEL 100000

// Sets default values
AMainGraph::AMainGraph()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	LineBatch = CreateDefaultSubobject<ULineBatchComponent>(TEXT("LineBatch"));
	LineBatch->SetupAttachment(RootComponent);
	LineBatch->SetRelativeLocation(FVector(0.0f));
	ActorSpeed = 100.0f;

	MeshBetweenActors = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ActorsMesh"));
	MeshBetweenActors->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Plane(
		TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	MeshBetweenActors->SetStaticMesh(Plane.Object);
	MeshBetweenActors->SetCastShadow(false);

	VertexMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("VertexMesh"));
	VertexMesh->SetupAttachment(RootComponent);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(
		TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	VertexMesh->SetStaticMesh(Cube.Object);
	VertexMesh->SetCastShadow(false);
	
}

// Called when the game starts or when spawned
void AMainGraph::BeginPlay()
{
	Super::BeginPlay();
	NewConjectureMatrix();
	InitializeConjectureMatrix();
	FillConjectureMatrix();
	AddStaticMeshes();
	CreateActorsToMove();
}

// Called every frame
void AMainGraph::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMainGraph::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	NewConjectureMatrix();
	InitializeConjectureMatrix();
	FillConjectureMatrix();
	
	LineBatch->Flush();
	
	for(int i = 0; i < Ribs.size(); ++i)
	{
		LineBatch->DrawPoint(Points[Ribs[i][0]], FLinearColor(1, 0, 1), 20, 1);
		LineBatch->DrawPoint(Points[Ribs[i][1]], FLinearColor(1, 0, 1), 20, 1);
		LineBatch->DrawLine(Points[Ribs[i][0]], Points[Ribs[i][1]], FLinearColor(0, 1, 0),
			0, 4);
	}
	
}

double AMainGraph::Random(const double Min, const double Max)
{
	return (double)(rand()) / RAND_MAX * (Max - Min) + Min;
}

void AMainGraph::NewConjectureMatrix()
{
	for(int i = 0; i < GraphPointsCountX; ++i)
	{
		for(int j = 0; j < GraphPointsCountY; ++j)
		{
			float RandX = Random(0, OffsetX) * 1;
			float RandY = Random(0, OffsetY) * 1;
			float RandZ = Random(0, OffsetZ) * 1;
			this->Points.push_back(FVector(RandX + 300.0f * i, RandY + 300.0f * j, RandZ + OffsetZ));
		}
	}

	for(int i = 0; i < GraphPointsCountX - 1; ++i)
	{
		for(int j = 0; j < GraphPointsCountY - 1; ++j)
		{
			std::vector<int> Temp;
			Temp.push_back(GraphPointsCountY * i + j);
			Temp.push_back(GraphPointsCountY * i + j + 1);
			
			if(rand() % 20 != 0)
			{
				this->Ribs.push_back(Temp);
			}
			
			Temp.clear();

			Temp.push_back(GraphPointsCountY * (GraphPointsCountX - 1) + j);
			Temp.push_back(GraphPointsCountY * (GraphPointsCountX - 1) + j + 1);
			
			if(rand() % 20 != 0)
			{
				this->Ribs.push_back(Temp);
			}
			
			Temp.clear();
			Temp.push_back(GraphPointsCountY * i + j);
			Temp.push_back(GraphPointsCountY * i + j + GraphPointsCountY);
			if(rand() % 20 != 0)
			{
				this->Ribs.push_back(Temp);
			}

			Temp.clear();
			Temp.push_back(GraphPointsCountY * i + j);
			Temp.push_back(GraphPointsCountY * i + j + GraphPointsCountY + 1);
			this->Ribs.push_back(Temp);
		}
	}
}

void AMainGraph::InitializeConjectureMatrix()
{
	std::vector<double> Row;
	for(int i = 0; i < (int)Points.size(); ++i)
	{
		Row.clear();
		for(int j = 0; j < (int)Points.size(); ++j)
		{
			Row.push_back(0.0);
		}
		Matrix.push_back(Row);
	}
}

void AMainGraph::FillConjectureMatrix()
{
	for(int i = 0; i < (int)Ribs.size(); ++i)
	{
		double X1, X2, Y1, Y2, Z1, Z2;
		int Point1 = Ribs[i][0];
		int Point2 = Ribs[i][1];
		X1 = Points[Point1].X;
		Y1 = Points[Point1].Y;
		Z1 = Points[Point1].Z;

		X2 = Points[Point2].X;
		Y2 = Points[Point2].Y;
		Z2 = Points[Point2].Z;

		double RibLength = sqrt(pow((X1 - X2), 2) + pow((Y1 - Y2), 2) + pow((Z1 - Z2), 2));
		Matrix[Point2][Point1] = RibLength;
		Matrix[Point1][Point2] = RibLength;
	}
}

std::vector<int> AMainGraph::GetShortPath(int start, int end)
{
	int N = Matrix.size();

	std::vector<float> vertex_labels;
	std::vector<bool> vertex_visited;
	std::vector<int> found_path;
	for (int i = 0; i < N; ++i) {
		vertex_labels.push_back(MAX_LABEL);
		vertex_visited.push_back(false);
		found_path.push_back(start);
	}

	vertex_labels[start] = 0;

	int current_vertex = start;
	do {
		// поиск соседа-вершины с минимальным расстоянием
		for (int i = 0; i < N; ++i) {
			// расстояние от текущей вершины до вершины i
			float h = (Matrix[current_vertex])[i];
			// если расстояние = 0, то пути нет
			if (h == 0) continue;
			// если расстояние не 0, то проверка на текущую метку вершины5
			if (h + vertex_labels[current_vertex] < vertex_labels[i]) {
				vertex_labels[i] = h + vertex_labels[current_vertex];
				found_path[i] = current_vertex;
			}
		}
		vertex_visited[current_vertex] = true;	// отмечаем вершину как посещенную

		// ищем соседа с минимальным расстоянием и заменяем индекс
		float minimal_pathsum;
		float min_index = vertex_labels.size() + 1;

		for (int i = 0; i < (int)vertex_labels.size(); ++i) {
			if ((!vertex_visited[i]) && (vertex_labels[i] < MAX_LABEL)) {
				minimal_pathsum = vertex_labels[i];
				min_index = i;
			}
		}
		current_vertex = min_index;
	} while (current_vertex != vertex_labels.size() + 1);

	// кратчайший путь
	std::vector<int> shortPath;
	// индексация от конца к началу
	int index = end;
	while (index != start) {
		shortPath.push_back(index);
		index = found_path[index];
	}
	shortPath.push_back(start);

	return shortPath;
	
}

void AMainGraph::CreateActorsToMove()
{
	int RandomPointStartIndex, RandomPointEndIndex;
	FVector Point;
	AFloatingActor *ActorToMove;
	
	for(int i = 0; i < BallCount; ++i)
	{
		RandomPointStartIndex = FMath::RandRange(0, Points.size() - 1);

		do
		{
			RandomPointEndIndex = FMath::RandRange(0, Points.size() - 1);
		} while (RandomPointEndIndex == RandomPointStartIndex);
		
		Point = Points[RandomPointStartIndex];
		
		FActorSpawnParameters Params;
		Params.Owner = this;
		
		ActorToMove = GetWorld()->SpawnActor<AFloatingActor>(Params);
		ActorToMove->AttachToComponent(LineBatch, FAttachmentTransformRules::KeepRelativeTransform);
		//ActorToMove->SetActorLocation(Point);
		//ActorToMove->SetStartPoint(RandomPointStartIndex);
		ActorToMove->SetActorSpeed(ActorSpeed);
		ActorToMove->SetPoints(Points);
		ActorToMove->SetNumberOfActors(BallCount);
		ActorToMove->CopyMatrix(Matrix);
		ActorToMove->SetActorPathColor(GenerateActorColor());
		Point = Points[RandomPointEndIndex];
		//ActorToMove->SetEndPoint(RandomPointEndIndex);
		ActorToMove->SetActorShortPath(GetShortPath(RandomPointStartIndex, RandomPointEndIndex));
		this->FloatingActors.push_back(ActorToMove);
	}
}

void AMainGraph::AddStaticMeshes()
{
	
	for(int i = 0; i < Points.size(); ++i)
	{
		SetVertex(Points[i]);	
	}
	
	for(int i = 0; i < Ribs.size(); ++i)
	{
		SetMeshBetweenActors(Points[Ribs[i][0]], Points[Ribs[i][1]]);
	}
}

void AMainGraph::SetMeshBetweenActors(FVector Start, FVector End)
{
	float Width = FVector::Distance(Start, End) / 100;
	float Height = 0.3;
	
	FVector Location(Start.X + (End.X - Start.X) / 2,
		Start.Y + (End.Y - Start.Y) / 2, Start.Z + (End.Z - Start.Z) / 2);
	
	const FRotator Rotator = FVector(End - Start).Rotation();
	
	MeshBetweenActors->AddInstance(FTransform(Rotator, Location, FVector(Width, Height, 1)));
}

void AMainGraph::SetVertex(FVector Position)
{
	float Width, Length, Height;
	Width = Length = Height = 0.35;
	
	FVector Location(Position.X, Position.Y, Position.Z);
	FRotator Rotator = FVector(Position).Rotation();
	VertexMesh->AddInstance(FTransform(Rotator, Location, FVector(Width, Height, Length)));
}

FLinearColor AMainGraph::GenerateActorColor()
{
	FLinearColor Color = FLinearColor(FMath::RandRange(0, 255), FMath::RandRange(0, 255),
		FMath::RandRange(0, 255)); 
	return Color;
}

