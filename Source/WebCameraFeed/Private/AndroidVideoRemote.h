//
//  AndroidVideoRemote.hpp
//  SariskaUnrealDemo_Index
//
//  Created by Dipak Sisodiya on 12/06/22.
//  Copyright © 2022 Epic Games, Inc. All rights reserved.
//
#pragma once
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include <Engine/Texture2D.h>

#if PLATFORM_ANDROID

class AndroidVideoRemote

{

public:
    

    static int returnNumber();
    
    static AndroidVideoRemote* get_instance();
    
    static AndroidVideoRemote* m_pInstance;
    
    AndroidVideoRemote();
    
    bool setup(int w, int h, bool mirrored);
    
    bool bIsInit = false;
    
    virtual ~AndroidVideoRemote();
    
    int number = 0;
    
    bool _mirror = false;
    
    int textureID;
    
    TArray<uint8>    copyPixels;
    
    void updatePixelsCB(unsigned char *isrc, int w, int h );
    
    void InitConverTable();
    
    void ConvertYUV2RGBA(unsigned char *src0, unsigned char *src1, unsigned char *dst_ori, int width, int height, bool mirror);
    
    void allocateDataRemote(int w, int h, EPixelFormat InFormat);
    
    TArray<uint8>    pixels;
    
    TWeakObjectPtr<UTexture2D>                remoteTexture;
    
    FRWLock  frwLock;
    
    UTexture* returnTexture();
    
    bool isTextureGenerated = false;
protected:
    
    void loadTexture();
    
    void deleteTexture();
    
    void close();
};
#endif

