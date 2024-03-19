// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ABWidgetComponent.h"
#include "ABUserWidget.h"

void UABWidgetComponent::InitWidget()
{
	Super::InitWidget();

	if (UABUserWidget* ABUserWidget = Cast <UABUserWidget>(GetWidget()))
	{
		ABUserWidget->SetOwningActor(GetOwner());
	}
}
