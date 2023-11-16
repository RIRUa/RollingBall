// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/InGameGameMode.h"
#include "Character/BallPlayer.h"

AInGameGameMode::AInGameGameMode() {
    this->DefaultPawnClass = ABallPlayer::StaticClass();
}
