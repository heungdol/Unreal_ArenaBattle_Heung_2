// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/ABCharacterBase.h"

#include "Animation/AnimMontage.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "ABComboActionData.h"
#include "ABCharacterControlData.h"

#include "Physics/ABCollision.h"
#include "Engine/DamageEvents.h"

#include "CharacterStat/ABCharacterStatComponent.h"
//#include "Components/WidgetComponent.h"

// 2_07
#include "UI/ABWidgetComponent.h"
#include "UI/ABHPBarWidget.h"

// Sets default values
AABCharacterBase::AABCharacterBase()
{
	// Rotation
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	// Capsule
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_ABCAPSULE);
	
	// Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 500.0f, 0);
	GetCharacterMovement()->JumpZVelocity = 700.0f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 10.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;

	// Mesh
	GetMesh()->SetRelativeLocationAndRotation
	(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
	
	static ConstructorHelpers::FObjectFinder <USkeletalMesh> CharacterMeshRef
	(TEXT("/Script/Engine.SkeletalMesh'/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard'"));

	if (CharacterMeshRef.Object != nullptr)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	// 클래스 정보는 마지막에 _C 붙임
	static ConstructorHelpers::FClassFinder <UAnimInstance> AnimInstanceClassRef
	(TEXT("/Game/ArenaBattle/Animation/ABP_ABCharacter.ABP_ABCharacter_C"));

	if (AnimInstanceClassRef.Class != nullptr)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

	static ConstructorHelpers::FObjectFinder<UABCharacterControlData> ShoulderDataRef 
	(TEXT ("/Script/ArenaBattle.ABCharacterControlData'/Game/ArenaBattle/CharacterControl/ABC_Shoulder.ABC_Shoulder'"));
	if (ShoulderDataRef.Object != nullptr)
	{
		CharacterControlManager.Add (ECharacterControlType::Shoulder, ShoulderDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UABCharacterControlData> QuaterDataRef 
	(TEXT ("/Script/ArenaBattle.ABCharacterControlData'/Game/ArenaBattle/CharacterControl/ABC_Quater.ABC_Quater'"));
	if (QuaterDataRef.Object != nullptr)
	{
		CharacterControlManager.Add (ECharacterControlType::Quater, QuaterDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder <UAnimMontage> ComboActionMontageRef
	(TEXT("/Script/Engine.AnimMontage'/Game/ArenaBattle/Animation/AM_ComboAttack.AM_ComboAttack'"));
	if (ComboActionMontageRef.Object != nullptr)
	{
		ComboActionMontage = ComboActionMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder <UABComboActionData> ComboActionDataRef
	(TEXT("/Script/ArenaBattle.ABComboActionData'/Game/ArenaBattle/CharacterAction/ABA_ComboActtack.ABA_ComboActtack'"));
	if (ComboActionDataRef.Object != nullptr)
	{
		ComboActionData = ComboActionDataRef.Object;
	}

	static ConstructorHelpers::FObjectFinder <UAnimMontage> DeadMontageRef
	(TEXT("/Script/Engine.AnimMontage'/Game/ArenaBattle/Animation/AM_Dead.AM_Dead'"));
	if (DeadMontageRef.Object != nullptr)
	{
		DeadMontage = DeadMontageRef.Object;
	}

	// Stat Component
	Stat = CreateDefaultSubobject <UABCharacterStatComponent> (TEXT ("Stat Component"));

	// Widget Component
	HPBar = CreateDefaultSubobject <UABWidgetComponent>(TEXT("Widget Component"));
	HPBar->SetupAttachment(GetMesh());
	HPBar->SetRelativeLocation(FVector(0, 0, 180.0f));

	static ConstructorHelpers::FClassFinder <UUserWidget> HPBarWidgetRef
	(TEXT("/Game/ArenaBattle/UI/WBP_HPBar.WBP_HPBar_C"));
	if (HPBarWidgetRef.Class != nullptr)
	{
		// 기본 Class 설정
		HPBar->SetWidgetClass(HPBarWidgetRef.Class);

		// 2D 스크린 설정
		HPBar->SetWidgetSpace(EWidgetSpace::Screen);

		// 사이즈 조절
		HPBar->SetDrawSize(FVector2D(150.0f, 15.0f));

		// 충돌 설정
		HPBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AABCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Stat->OnHPZero.AddUObject(this, &AABCharacterBase::SetDead);
}

void AABCharacterBase::SetCharacterControlData(const UABCharacterControlData* CharacterControlData)
{
	// Pawn
	bUseControllerRotationYaw = CharacterControlData->bUseControllerRotationYaw;

	// CharacterMovement
	GetCharacterMovement ()->bOrientRotationToMovement = CharacterControlData->bOrientRotationToMovement;
	GetCharacterMovement ()->bUseControllerDesiredRotation = CharacterControlData->bUseControllerDesiredRotation;
	GetCharacterMovement ()->RotationRate = CharacterControlData->RotationRate;
}

void AABCharacterBase::SetupCharacterWidget(UABUserWidget* InUserWidget)
{
	if (UABHPBarWidget* HPBarWidget = Cast <UABHPBarWidget>(InUserWidget))
	{
		HPBarWidget->SetMaxHP(Stat->GetMaxHP ());
		HPBarWidget->UpdateHPBar(Stat->GetCurrentHP());

		// 2_07 HP 변환 시 위젯을 업데이트 하도록 설정
		Stat->OnHPChanged.AddUObject(HPBarWidget, &UABHPBarWidget::UpdateHPBar);
	}
}

void AABCharacterBase::ProcessComboCommand()
{
	if (CurrentCombo == 0)
	{
		ComboActionBegin();
		return;
	}

	if (!ComboTimerHandle.IsValid())
	{
		HasNextComboCommand = false;
	}
	else
	{
		HasNextComboCommand = true;
	}
}

void AABCharacterBase::ComboActionBegin()
{
	// Combo Start
	CurrentCombo = 1;

	// Movement Setting
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	// Animation Setting
	const float AttackSpeedRate = 1.0f;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(ComboActionMontage, AttackSpeedRate);

	FOnMontageEnded EndDeletage;
	EndDeletage.BindUObject(this, &AABCharacterBase::ComboActionEnd);
	AnimInstance->Montage_SetEndDelegate(EndDeletage, ComboActionMontage);

	// 타임 핸들러 초기화
	ComboTimerHandle.Invalidate();
	SetComboCheckTimer();

}

void AABCharacterBase::ComboActionEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	ensure(CurrentCombo != 0);

	// Reset Combo
	CurrentCombo = 0;

	// Enable Move
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void AABCharacterBase::SetComboCheckTimer()
{
	int32 ComboIndex = CurrentCombo - 1;

	// IsValidIndex 이 함수 유용히 사용할 수 있을 것 같음
	ensure(ComboActionData->EffectiveFrameCount.IsValidIndex(ComboIndex));

	const float AttackSpeedRate = 1.0f;
	float ComboEffecttiveTime = (ComboActionData->EffectiveFrameCount[ComboIndex] / ComboActionData->FrameRate) / AttackSpeedRate;

	if (ComboEffecttiveTime > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &AABCharacterBase::ComboCheck, ComboEffecttiveTime, false);
	}
}

void AABCharacterBase::ComboCheck()
{
	ComboTimerHandle.Invalidate();

	if (HasNextComboCommand)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		// Min Max -> Clamp
		CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, ComboActionData->MaxComboCount);

		FName NextSection = *FString::Printf(TEXT("%s%d"), *ComboActionData->MontageSectionNamePrefix, CurrentCombo);

		AnimInstance->Montage_JumpToSection(NextSection, ComboActionMontage);

		SetComboCheckTimer();

		HasNextComboCommand = false;
	}
	else
	{

	}
}

void AABCharacterBase::AttackHitCheck()
{
	FHitResult OutHitResult;
	// 인자: 콜리전을 분석할 때 사용되는 태그 정보, 복잡한 형태의 충돌체를 감치할 것인지, 무시할 액터들
	// SCENE_QUERY_STAT: 언리얼 엔진에서 제공하는 분석 툴
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);

	const float AttackRange = 40.0f;
	const float AttackRadius = 50.0f;
	const float AttackDamage = 30.0f;

	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * AttackRange;

	bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, CCHANNEL_ABACTION, FCollisionShape::MakeSphere (AttackRadius), Params);

	if (HitDetected)
	{
		FDamageEvent DamageEvent;
		OutHitResult.GetActor()->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
	}

#if ENABLE_DRAW_DEBUG

	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	float CapsuleHalfHeight = AttackRange * 0.5f;
	FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;

	// 인자: 월드, 시작점, 캡술 반 높이, 반지름, 방향, 색깔, 계속해서 유지할 것인지, 언제까지 유지할 것인지
	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat (), DrawColor, false, 5.0f);

#endif
}

float AABCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// EventInstigator: 가해자
	// DamageCauser: 가해자가 사용한 무기 / 가해자가 빙의한 폰
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	// 2_07
	Stat->ApplyDamage(DamageAmount);

	return DamageAmount;
}

void AABCharacterBase::SetDead()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	PlayDeadAnimation();
	SetActorEnableCollision(false);

	HPBar->SetHiddenInGame(true);
}

void AABCharacterBase::PlayDeadAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	
	// 인자: 블렌드 아웃할 때 걸리는 시간
	// 0.0f: 즉시 중지
	// 1.0f: 1초 동안 서서히 섞으며 중지
	AnimInstance->StopAllMontages(0.0f);

	// 인자: 플레이할 몽타주, 애니메이션 속도
	AnimInstance->Montage_Play(DeadMontage, 1.0f);
}
