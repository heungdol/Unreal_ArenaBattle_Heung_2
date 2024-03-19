// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ABUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	//UABUserWidget(const FObjectInitializer& ObjectInitializer);
	
	FORCEINLINE void SetOwningActor(AActor* NewActor) { OwningActor = NewActor; }

protected:
	
	UPROPERTY (VisibleAnywhere, BlueprintReadOnly, Category = Actor)
	TObjectPtr <AActor> OwningActor;

};
