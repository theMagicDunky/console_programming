// Fill out your copyright notice in the Description page of Project Settings.

#include "TextureGenerator.h"


// Sets default values
ATextureGenerator::ATextureGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATextureGenerator::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATextureGenerator::PostInitializeComponents()
{
	
}

void ATextureGenerator::Init(UStaticMeshComponent * staticMesh, int32 materialIndex, int32 w, int32 h)
{
	if (!staticMesh)
		return;

	width = w;
	height = h;

	if (!width || !height)
		return;

	// texture setup
	myTexture = UTexture2D::CreateTransient(width, height);
}

// Called every frame
void ATextureGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

