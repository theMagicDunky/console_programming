// Fill out your copyright notice in the Description page of Project Settings.

#include "TextureGenerator.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h "
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"

// Sets default values
ATextureGenerator::ATextureGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

ATextureGenerator::~ATextureGenerator()
{
	if (pixels)
		delete[] pixels;
	if(updateTextureRegion)
		delete updateTextureRegion;
}

// Called when the game starts or when spawned
void ATextureGenerator::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATextureGenerator::PostInitializeComponents()
{
	Super::PostInitializeComponents();
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
	// no compression pls
	myTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	myTexture->SRGB = 0;
	// stops garbage collection
	myTexture->AddToRoot();
	myTexture->UpdateResource();

	updateTextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, width, height);

	numPixels = width * height;
	if (pixels)
		delete[] pixels;
	pixels = new TexturePixel[numPixels];

	for (int i = 0; i < numPixels; ++i)
	{
		pixels[i] = {0, 0, 255, 255};
	}

	myTexture->UpdateTextureRegions(
		0, 1, updateTextureRegion, width * sizeof(TexturePixel), sizeof(TexturePixel),
		reinterpret_cast<uint8*>(pixels)
	);

	UMaterialInstanceDynamic* material = staticMesh->CreateAndSetMaterialInstanceDynamic(materialIndex);
	material->SetTextureParameterValue("texture", myTexture);
}
// Called every frame
void ATextureGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

