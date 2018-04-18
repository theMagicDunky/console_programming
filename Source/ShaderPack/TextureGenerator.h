// Fill out your copyright notice in the Description page of Project Settings.

// many thanks fam - https://wiki.unrealengine.com/Procedural_Materials

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Engine/Texture2D.h"
#include "TextureGenerator.generated.h"

struct TexturePixel
{
	uint8 b;
	uint8 g;
	uint8 r;
	uint8 a;
};

UCLASS()
class SHADERPACK_API ATextureGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATextureGenerator();
	~ATextureGenerator();

protected:
	const int numBubbleTextures = 1;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	void generateNoise();
	bool saveTexture();
	void setPixel(int h, int w, TexturePixel col);

	UFUNCTION(BlueprintCallable, Category = "TextureGen")
	void Init(UStaticMeshComponent* staticMesh, int32 materialIndex, int32 w, int32 h);
	UFUNCTION(BlueprintCallable, Category = "TextureGen")
	void GenNoiseTexture();
	UFUNCTION(BlueprintCallable, Category = "TextureGen")
	void GenNewBubbleTextures(int radius, int numBubbles);
	UFUNCTION(BlueprintCallable, Category = "TextureGen")
	void newTexture();
	UFUNCTION(BlueprintCallable, Category = "TextureGen")
	void GenCloudTexture();

	double smoothNoise(TexturePixel* noisePixels, double x, double y);
	double turbulence(TexturePixel *noisePixels, double x, double y, double size);

	void GenCircleTexture(int radius, int numBubbles, FName texture);

	UMaterialInstanceDynamic* myMaterial;
	UTexture2D *myTextures[256];
	UTexture2D *currentTexture;
	FUpdateTextureRegion2D* updateTextureRegion;
	UPackage *package[256];
	UPackage *currentPackage;

	int32 width, height, numPixels;
	uint8 texID = 0;

	TexturePixel* pixels;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;	
	
};
