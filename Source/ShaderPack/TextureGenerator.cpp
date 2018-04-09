// Fill out your copyright notice in the Description page of Project Settings.

#include "TextureGenerator.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h "
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "Runtime/CoreUObject/Public/UObject/Package.h"
#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"

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

	numPixels = width * height;
	if (pixels)
		delete[] pixels;
	pixels = new TexturePixel[numPixels];

	// thanks fam
	// http://isaratech.com/save-a-procedurally-generated-texture-as-a-new-asset/

	FString packageName = TEXT("/Game/ProceduralTextures/circleTex");
	package = CreatePackage(NULL, *packageName);
	package->FullyLoad();

	// texture setup
	myTexture = NewObject<UTexture2D>(package, TEXT("circleTex"), RF_Public | RF_Standalone | RF_MarkAsRootSet);
	myTexture->AddToRoot();
	myTexture->PlatformData = new FTexturePlatformData();
	myTexture->PlatformData->SizeX = width; myTexture->PlatformData->SizeY = height;
	myTexture->PlatformData->NumSlices = 1;
	myTexture->PlatformData->PixelFormat = EPixelFormat::PF_B8G8R8A8;

	generateNoise();
	bool saved = saveTexture(packageName);

	myMaterial = staticMesh->CreateAndSetMaterialInstanceDynamic(materialIndex);

	UE_LOG(LogTemp, Warning, TEXT("%s"), saved ? TEXT("wow it did it") : TEXT("wow it didnt do it"));
	if (saved)
	{
		myMaterial->SetTextureParameterValue("bubbleTexture1", myTexture);
		UE_LOG(LogTemp, Warning, TEXT("pls work"));
	}
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

bool ATextureGenerator::saveTexture(FString packageName)
{
	FTexture2DMipMap* mip = new(myTexture->PlatformData->Mips) FTexture2DMipMap();
	mip->SizeX = width; mip->SizeY = height;

	mip->BulkData.Lock(LOCK_READ_WRITE);
	uint8* textureData = (uint8*)mip->BulkData.Realloc(width * height * 4);
	FMemory::Memcpy(textureData, pixels, sizeof(TexturePixel) * width * height);
	mip->BulkData.Unlock();

	myTexture->Source.Init(width, height, 1, 1, ETextureSourceFormat::TSF_BGRA8, reinterpret_cast<uint8*>(pixels));
	myTexture->UpdateResource();
	package->MarkPackageDirty();
	FAssetRegistryModule::AssetCreated(myTexture);

	FString packageFilename = FPackageName::LongPackageNameToFilename(packageName, FPackageName::GetAssetPackageExtension());
	return UPackage::SavePackage(package, myTexture, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *packageFilename, GError, nullptr, true, true, SAVE_NoError);
}

// Called every frame
void ATextureGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
