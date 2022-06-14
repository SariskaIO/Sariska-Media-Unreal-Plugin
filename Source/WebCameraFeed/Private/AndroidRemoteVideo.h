//
//  AndroidRemoteVideo.hpp
//  SariskaUnrealDemo_Index
//
//  Created by Dipak Sisodiya on 14/06/22.
//  Copyright © 2022 Epic Games, Inc. All rights reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "BaseVideoGrabber.h"
#if PLATFORM_ANDROID

class AndroidRemoteVideo: public BaseVideoGrabber
{
public:
    AndroidRemoteVideo();
    virtual ~AndroidRemoteVideo();
    
    void setDeviceID(int deviceID) override;
    
    TArray<FVideoDevice>    listDevices() const override;

    bool isFrameNew() const override;

    int getHeight() const override;

    int getWidth() const override;
    
    bool switchBackAndFront() override;

    int getBackCamera() const override;

    int getFrontCamera() const override;
    
    void close() override;

    void update() override;

    bool setup(int w, int h, bool mirrored) override;

    void updatePixelsCB(unsigned char *isrc, int w, int h );
    
protected:

    void loadTexture();
    void deleteTexture();

    void pause() override;
    void resume() override;
    
    bool newFrame = false;
    bool bHavePixelsChanged = false;
    int width, height;
    int device = 0;
    bool bIsInit = false;
    int fps  = -1;

    int textureID;

    TArray<uint8>    copyPixels;
    bool  _mirror;
};
#endif
