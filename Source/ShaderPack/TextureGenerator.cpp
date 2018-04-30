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

void ATextureGenerator::generateTiledNoise()
{
	int size = 128;
	float freq = 1 / 32.0f, octs = 5;

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			uint8 num = 0;
			for (int o = 0; o < octs; ++o)
			{
				num += FMath::Pow(0.5f, o) * perlinNoise(x*FMath::Pow(2, o), y*FMath::Pow(2, o), (size*(int)freq)*FMath::Pow(2, o));
			}
			pixels[(y*height) + x] = { num, num, num, 255 };
		}
	}
}

float ATextureGenerator::perlinNoise(float x, float y, int per)
{
	int intX = (int)x, intY = (int)y;
	uint8 perm[512];
	float dirs[256][2];
	for (int i = 0; i < 256; ++i)
	{
		perm[i] = perm[i + 255] = FMath::RandRange(0, 255);
		dirs[i][0] = FMath::Cos(i * 2.0f * PI / 256.0f);
		dirs[i][1] = FMath::Sin(i * 2.0f * PI / 256.0f);
	}

	return surflet(per, intX + 0, intY + 0, x, y, perm, dirs) + surflet(per, intX + 1, intY + 0, x, y, perm, dirs) +
		surflet(per, intX + 0, intY + 1, x, y, perm , dirs) + surflet(per, intX + 1, intY + 1, x, y, perm, dirs);
}

float ATextureGenerator::surflet(int per, float gridX, float gridY, float x, float y, uint8 *perm, float dirs[][2])
{
	float distX = FMath::Abs(x - gridX), distY = FMath::Abs(y - gridY);
	float polyX = 1 - 6 * FMath::Pow(distX, 5) + 15 * FMath::Pow(distX, 4) - 10 * FMath::Pow(distX, 3);
	float polyY = 1 - 6 * FMath::Pow(distY, 5) + 15 * FMath::Pow(distY, 4) - 10 * FMath::Pow(distY, 3);
	int hashed = perm[perm[int(gridX) % per] + int(gridY) % per];
	float grad = 1.0f;//(x - gridX)*dirs[hashed][0] + (y - gridY)*dirs[hashed][1];
	return polyX * polyY * grad;
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

	currentPackage = package[texID];
	FString packageName = TEXT("/Game/ProceduralTextures/circleTex");
	currentPackage = CreatePackage(NULL, *packageName);
	currentPackage->FullyLoad();

	currentTexture = myTextures[texID++];
	// texture setup
	currentTexture = NewObject<UTexture2D>(currentPackage, TEXT("circleTex"), RF_Public | RF_Standalone | RF_MarkAsRootSet);
	currentTexture->AddToRoot();
	currentTexture->PlatformData = new FTexturePlatformData();
	currentTexture->PlatformData->SizeX = width; currentTexture->PlatformData->SizeY = height;
	currentTexture->PlatformData->NumSlices = 1;
	currentTexture->PlatformData->PixelFormat = EPixelFormat::PF_B8G8R8A8;

	//generateNoise();
	//bool saved = saveTexture();

	myMaterial = staticMesh->CreateAndSetMaterialInstanceDynamic(materialIndex);

	//UE_LOG(LogTemp, Warning, TEXT("%s"), saved ? TEXT("wow it did it") : TEXT("wow it didnt do it"));
	//if (saved)
	//{
	//	myMaterial->SetTextureParameterValue("bubbleTexture1", currentTexture);
	//	UE_LOG(LogTemp, Warning, TEXT("pls work"));
	//}
}
void ATextureGenerator::GenNoiseTexture()
{
	generateNoise();
	//generateTiledNoise();

	currentTexture->UpdateTextureRegions(
		0, 1, updateTextureRegion, width * sizeof(TexturePixel), sizeof(TexturePixel),
		reinterpret_cast<uint8*>(pixels)
	);

	myMaterial->SetTextureParameterValue("noiseTexture", currentTexture);
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

	// urrentTexture->UpdateTextureRegions(
	// 0, 1, updateTextureRegion, width * sizeof(TexturePixel), sizeof(TexturePixel),
	// reinterpret_cast<uint8*>(pixels)
	saveTexture();
	myMaterial->SetTextureParameterValue(texture, currentTexture);
}

bool ATextureGenerator::saveTexture()
{
	FTexture2DMipMap* mip = new(currentTexture->PlatformData->Mips) FTexture2DMipMap();
	mip->SizeX = width; mip->SizeY = height;

	mip->BulkData.Lock(LOCK_READ_WRITE);
	uint8* textureData = (uint8*)mip->BulkData.Realloc(width * height * 4);
	FMemory::Memcpy(textureData, pixels, sizeof(TexturePixel) * width * height);
	mip->BulkData.Unlock();

	currentTexture->Source.Init(width, height, 1, 1, ETextureSourceFormat::TSF_BGRA8, reinterpret_cast<uint8*>(pixels));
	currentTexture->UpdateResource();
	currentPackage->MarkPackageDirty();
	FAssetRegistryModule::AssetCreated(currentTexture);

	FString filePath = TEXT("/Game/ProceduralTextures/tex_"); filePath.Append(FString::FromInt(texID));
	FString packageFilename = FPackageName::LongPackageNameToFilename(filePath, FPackageName::GetAssetPackageExtension());
	return UPackage::SavePackage(currentPackage, currentTexture, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *packageFilename, GError, nullptr, true, true, SAVE_NoError);
}

void ATextureGenerator::newTexture()
{
	currentTexture = myTextures[++texID];
	currentPackage = package[texID];
	FString packageName = TEXT("/Game/ProceduralTextures/tex_");
	packageName += FString::FromInt(texID);
	currentPackage = CreatePackage(NULL, *packageName);
	currentPackage->FullyLoad();
	// texture setup
	FString name = TEXT("tex_"); name += FString::FromInt(texID);
	currentTexture = NewObject<UTexture2D>(currentPackage, *name, RF_Public | RF_Standalone | RF_MarkAsRootSet);
	currentTexture->AddToRoot();
	currentTexture->PlatformData = new FTexturePlatformData();
	currentTexture->PlatformData->SizeX = width; currentTexture->PlatformData->SizeY = height;
	currentTexture->PlatformData->NumSlices = 1;
	currentTexture->PlatformData->PixelFormat = EPixelFormat::PF_B8G8R8A8;
}

// http://lodev.org/cgtutor/randomnoise.html#Clouds
void ATextureGenerator::GenCloudTexture(float turbulenceSize)
{
	TexturePixel* noisePixels = new TexturePixel[numPixels];

	for (int h = 0; h < height; ++h)
	{
		for (int w = 0; w < width; ++w)
		{
			uint8 num = rand() % 255;
			noisePixels[((h*height)) + (w)] = { num, num, num, 255 };
		}
	}

	for (int h = 0; h < height; ++h)
	{
		for (int w = 0; w < width; ++w)
		{
			//uint8 num = smoothNoise(noisePixels, w / 2.0, h / 2.0);
			//uint8 num = smoothNoise(noisePixels, (double)w / 16.0, (double)h / 16.0);
			uint8 num = turbulence(noisePixels, w, h, turbulenceSize);
			//noisePixels[((h*height)) + (w)] = { num, num, num, 255 };
			pixels[((h*height)) + (w)] = { num, num, num, 255 };
		}
	}

	saveTexture();
}

double ATextureGenerator::smoothNoise(TexturePixel* noisePixels, double x, double y)
{
	double fractX = x - int(x);
	double fractY = y - int(y);

	int x1 = (int(x) + width) % width;
	int y1 = (int(y) + height) % height;

	int x2 = (x1 + width - 1) % width;
	int y2 = (y1 + height - 1) % height;

	double value = 0.0;
	value += fractX * fractY * noisePixels[(y1 * height) + x1].b;
	value += (1 - fractX) * fractY * noisePixels[(y1 * height) + x2].b;
	value += fractX * (1 - fractY) * noisePixels[(y2 * height) + x1].b;
	value += (1- fractX) * (1 - fractY) * noisePixels[(y2 * height) + x2].b;

	return value;
}

double ATextureGenerator::turbulence(TexturePixel *noisePixels, double x, double y, double size)
{
	double value = 0.0, intialSize = size;
	uint8 counter = 0;

	while (size >= 1)
	{
		value += smoothNoise(noisePixels, x / size, y / size);
		size /= 2.0; ++counter;
	}

	return (value / counter);
}

// Called every frame
void ATextureGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
