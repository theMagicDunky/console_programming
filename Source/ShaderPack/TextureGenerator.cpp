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

void ATextureGenerator::generateNoise()
{
	for (int h = 0; h < height; ++h)
	{
		for (int w = 0; w < width; ++w)
		{
			uint8 num = rand() % 255;
			pixels[(h*height) + w] = { num, num, num, 255 };
		}
	}
}

void ATextureGenerator::setPixel(int h, int w, TexturePixel col)
{
	pixels[(w*width) + h] = col;
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

	generateNoise();

	myTexture->UpdateTextureRegions(
		0, 1, updateTextureRegion, width * sizeof(TexturePixel), sizeof(TexturePixel),
		reinterpret_cast<uint8*>(pixels)
	);

	myMaterial = staticMesh->CreateAndSetMaterialInstanceDynamic(materialIndex);
}
void ATextureGenerator::GenNoiseTexture()
{
	generateNoise();

	myTexture->UpdateTextureRegions(
		0, 1, updateTextureRegion, width * sizeof(TexturePixel), sizeof(TexturePixel),
		reinterpret_cast<uint8*>(pixels)
	);

	myMaterial->SetTextureParameterValue("noiseTexture", myTexture);
}

void ATextureGenerator::GenNewBubbleTextures(int radius, int numBubbles)
{
	for (int i = 0; i < numBubbleTextures; ++i)
	{
		FString textureName = "bubbleTexture" + FString::FromInt(i);
		GenCircleTexture(radius, numBubbles, FName(*textureName));
	}
}

void ATextureGenerator::GenCircleTexture(int radius, int numBubbles, FName texture)
{
	int i;

	for (i = 0; i < numPixels; ++i)
		pixels[i] = { 0, 0, 0, 255 };

	for (i = 0; i < numBubbles; ++i)
	{
		int xLoc = rand() % width;
		int yLoc = rand() % height;
	
		for (int j = 0; j <= radius; ++j)
		{
			int d = FMath::CeilToInt(FMath::Sqrt(radius * radius - j * j));
			for (int k = 0; k <= d; ++k)
			{
				//FMath::FloorToInt((float)k+1 / (float)d+1)
				TexturePixel col = { 128, 128, 128, 255};
				setPixel(j + xLoc, k + yLoc, col);
				setPixel(-j + xLoc, k + yLoc, col);
				setPixel(-j + xLoc, -k + yLoc, col);
				setPixel(j + xLoc, -k + yLoc, col);
			}
		}
	}

	myTexture->UpdateTextureRegions(
		0, 1, updateTextureRegion, width * sizeof(TexturePixel), sizeof(TexturePixel),
		reinterpret_cast<uint8*>(pixels)
	);

	myMaterial->SetTextureParameterValue(texture, myTexture);
}

// Called every frame
void ATextureGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

