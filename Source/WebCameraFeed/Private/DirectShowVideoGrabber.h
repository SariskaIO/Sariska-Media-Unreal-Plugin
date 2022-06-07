// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseVideoGrabber.h"

#if PLATFORM_WINDOWS

#include "Windows/AllowWindowsPlatformTypes.h"
#include "videoInput.h"
#include "Windows/HideWindowsPlatformTypes.h"

class DirectShowVideoGrabber: public BaseVideoGrabber
{
public:
	DirectShowVideoGrabber();
	virtual ~DirectShowVideoGrabber();

	void setDeviceID(int deviceID);

	TArray<FVideoDevice>	listDevices() const override;

	bool isFrameNew() const override;

	void close() override;

	void update() override;

	bool setup(int w, int h, bool mirrored) override;

	int getHeight() const override;

	int getWidth() const override;
    
    bool switchBackAndFront() override { return false; }

	int getBackCamera() const override { return 0; }
	
	int getFrontCamera() const override { return 0; }
    

protected:
    
    void pause() override; 
    void resume() override;
	int width;
	int height;
	bool bChooseDevice;
	bool bGrabberInited;
	bool bDoWeNeedToResize;
	bool bIsFrameNew;
	int 					device;
	videoInput 				VI;
};
#endif
