// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ABHPBarWidget.h"

#include "Components/ProgressBar.h"

#include "Interface/ABCharacterWidgetInterface.h"

UABHPBarWidget::UABHPBarWidget(const FObjectInitializer& ObjectInitializer)
	// 부모 클래스에게 인자 전달
	: Super (ObjectInitializer)
{
}

void UABHPBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HPProgressBar = Cast <UProgressBar>(GetWidgetFromName(TEXT("PbHPBar")));
	ensure(HPProgressBar != nullptr);

	if (IABCharacterWidgetInterface* CharacterWidget = Cast <IABCharacterWidgetInterface>(OwningActor))
	{
		CharacterWidget->SetupCharacterWidget(this);
	}
}

void UABHPBarWidget::UpdateHPBar(float NewCurrentHP)
{
	ensure(NewCurrentHP > 0.0f);

	if (HPProgressBar != nullptr)
	{
		HPProgressBar->SetPercent(NewCurrentHP / MaxHP);
	}
}
