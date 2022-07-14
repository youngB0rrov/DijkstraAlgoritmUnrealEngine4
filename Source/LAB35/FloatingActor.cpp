// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingActor.h"

#include "Components/LineBatchComponent.h"
#define MAX_LABEL 100000

// Sets default values
AFloatingActor::AFloatingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ActorCanBeMoved = false;
	CurrentVertex = 0;
	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	VisualMesh->SetupAttachment(RootComponent);
	
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> ActorMaterial(
		TEXT("Material'/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial'"));

	UMaterialInstanceDynamic* Material =
		UMaterialInstanceDynamic::Create(ActorMaterial.Object, nullptr);
	
	VisualMesh->SetCastShadow(false);
	VisualMesh->SetMaterial(0, Material);
	
	LineBatch = CreateDefaultSubobject<ULineBatchComponent>(TEXT("ActorLine"));
	LineBatch->SetupAttachment(RootComponent);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeVisualAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));

	if (CubeVisualAsset.Succeeded())
	{
		VisualMesh->SetStaticMesh(CubeVisualAsset.Object);
		VisualMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	}

}

// Called when the game starts or when spawned
void AFloatingActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFloatingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(!ActorCanBeMoved && CurrentVertex == 0)
	{
		CreateActorPath();
	} else
	{
		FVector Direction = NextActorPosition - CurrentActorPosition;
		Direction.Normalize();
		FVector NewActorPosition = GetActorLocation() + Direction * ActorSpeed * DeltaTime;
		SetActorLocation(NewActorPosition);
		
		if (FVector::DistSquared(NewActorPosition, CurrentActorPosition) > FVector::DistSquared(NextActorPosition,
			CurrentActorPosition))
		{
			SetActorLocation(NextActorPosition);
		
			if(CurrentVertex == ActorShortPath.size() - 2)
			{
				ActorCanBeMoved = false;
				LineBatch->Flush();
				StartPoint = ActorShortPath[ActorShortPath.size() - 1];
				CurrentVertex = 0;
				ActorShortPath.clear();
				
				do
				{
					EndPoint = FMath::RandRange(0, ActorPoints.size() - 1);
				} while (StartPoint == EndPoint);
				ActorShortPath = getShortPathWhenFinished(EndPoint, StartPoint);
			}
			else
			{
				CurrentVertex++;
				CurrentActorPosition = ActorPoints[ActorShortPath[CurrentVertex]];
				NextActorPosition = ActorPoints[ActorShortPath[CurrentVertex + 1]];
			}
			
		}
	}
}

void AFloatingActor::SetStartPoint(const int PointIndex)
{
	this->StartPoint = PointIndex;
}

void AFloatingActor::SetEndPoint(const int PointIndex)
{
	this->EndPoint = PointIndex;
}

void AFloatingActor::SetActorShortPath(const std::vector<int> ShortPath)
{
	this->ActorShortPath = ShortPath;
}

void AFloatingActor::SetActorSpeed(const float Speed)
{
	this->ActorSpeed = Speed;
}

void AFloatingActor::CreateActorPath()
{
	ActorCanBeMoved = true;
	CurrentActorPosition = ActorPoints[ActorShortPath[CurrentVertex]];
	NextActorPosition = ActorPoints[ActorShortPath[CurrentVertex + 1]];
	SetActorLocation(CurrentActorPosition);
	
	for(int i = 0; i < ActorShortPath.size() - 1; i++)
	{
		LineBatch->DrawLine(ActorPoints[ActorShortPath[i]], ActorPoints[ActorShortPath[i + 1]],
		ActorPathColor, 7, 8);
	}
}

void AFloatingActor::SetPoints(std::vector<FVector> GraphPoints)
{
	this->ActorPoints = GraphPoints;
}

std::vector<int> AFloatingActor::getShortPathWhenFinished(int start, int end)
{
	int N = CopiedMatrix.size();

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
			float h = (CopiedMatrix[current_vertex])[i];
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

