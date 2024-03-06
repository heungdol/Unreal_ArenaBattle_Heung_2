// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_AttackHitCheck.h"

#include "Character/ABCharacterBase.h"

void UAnimNotify_AttackHitCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp != nullptr)
	{
		IABAnimationAttackInterface* AttackPawn
			= Cast <IABAnimationAttackInterface>(MeshComp->GetOwner());
		
		if (AttackPawn != nullptr)
		{
			AttackPawn->AttackHitCheck();
		}
	}
}
