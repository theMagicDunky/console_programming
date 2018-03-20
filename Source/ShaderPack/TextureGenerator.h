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
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	UFUNCTION(BlueprintCallable, Category = "TextureGen")
	void Init(UStaticMeshComponent* staticMesh, int32 materialIndex, int32 w, int32 h);

	UMaterialInstanceDynamic* myMaterial;
	UTexture2D* myTexture;
	FUpdateTextureRegion2D* updateTextureRegion;

	int32 width, height, numPixels;

	TexturePixel* pixels;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
