// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ABCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHPZeroDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHPChangedDelegate, float /*CurrentHP*/);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLE_API UABCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UABCharacterStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Tick 함수를 사용하지 않음으로써 부하를 줄임
//public:	
//	// Called every frame
//	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


public:
	FOnHPZeroDelegate OnHPZero;
	FOnHPChangedDelegate OnHPChanged;

	// FORCEINLINE: 
	FORCEINLINE float GetMaxHP() { return MaxHP; }

	FORCEINLINE float GetCurrentHP() { return CurrentHP; }

	float ApplyDamage(float InDamage);

protected:

	void SetHP(float NewHP);


	UPROPERTY(VisibleInstanceOnly, Category = Stat)
	float MaxHP;
		
	// Transient: 매번 새롭게 사용되므로, 디스크에 저장할 필요가 없음
	// -> 불필요한 공간이 낭비되지 않드록 해당 키워드 사용
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat)
	float CurrentHP;
};
