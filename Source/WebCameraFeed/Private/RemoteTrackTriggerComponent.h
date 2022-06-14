//
//  RemoteTrackTriggerComponent.hpp
//  SariskaUnrealDemo_Index
//
//  Created by Dipak Sisodiya on 12/06/22.
//  Copyright © 2022 Epic Games, Inc. All rights reserved.
//
#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "RemoteTrackTriggerComponent.generated.h"

UCLASS( ClassGroup=(WebCamera), meta=(BlueprintSpawnableComponent), Config=Game )

class URemoteTrackTriggerComponent : public USceneComponent{
    GENERATED_BODY()
    
    static void GetSetGo();
    
public:
    int someVar;
};
