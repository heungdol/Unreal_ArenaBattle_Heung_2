// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ABGameModeBase.h"
#include "Player/ABPlayerController.h"

AABGameModeBase::AABGameModeBase()
{
	// 블루프린트 액터의 클래스를 가져온다
	// _C
	//static ConstructorHelpers::FClassFinder <APawn> ThirdPersonClassRef
	//(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter.BP_ThirdPersonCharacter_C"));

	static ConstructorHelpers::FClassFinder <APawn> ThirdPersonClassRef
	(TEXT("/Game/ArenaBattle/Blueprint/BP_ABCharacterPlayer.BP_ABCharacterPlayer_C"));

	// AB플레이어 컨트롤러를 가져온다
	static ConstructorHelpers::FClassFinder < APlayerController > PlayerControllerClassRef
	(TEXT("/Script/ArenaBattle.ABPlayerController"));

	if (ThirdPersonClassRef.Class != nullptr)
	{
		DefaultPawnClass = ThirdPersonClassRef.Class;
	}

	if (PlayerControllerClassRef.Class != nullptr)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}
}

