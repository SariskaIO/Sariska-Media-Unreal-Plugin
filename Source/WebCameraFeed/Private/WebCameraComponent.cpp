// Fill out your copyright notice in the Description page of Project Settings.

#include "WebCameraComponent.h"
#include "VideoGrabberPool.h"


UTexture* remoteTexturePointer;
// Sets default values for this component's properties
UWebCameraComponent::UWebCameraComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	DeviceId.selectedDevice = 0;
	requestedWidth = 640;
	requestedHeight = 480;
    MirroredVideo = true;
}


// Called when the game starts
void UWebCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	TArray<FString> devices = ListDevices();

	if (DeviceId.selectedDevice >= devices.Num()) {
		DeviceId.selectedDevice = devices.Num() - 1;
	}

	if (DeviceId.selectedDevice < 0) DeviceId.selectedDevice = 0;
    
	currentVideoGrabber = VideoGrabberPool::GetVideoGrabber(DeviceId.selectedDevice, requestedWidth, requestedHeight, MirroredVideo);
        
    
    remoteVideoGrabber = VideoGrabberPool::GetVideoGrabberRemote(42, requestedWidth, requestedHeight, MirroredVideo);
	
}

void UWebCameraComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	if (currentVideoGrabber.IsValid()) {
		VideoGrabberPool::ReleaseVideoGrabber(currentVideoGrabber);
	}
}


bool UWebCameraComponent::SaveAsImage(const FString& FileName) {
    if (currentVideoGrabber.IsValid()) {
        const FString ScreenShotPath = FPaths::GetPath(FileName);
        if ( IFileManager::Get().MakeDirectory(*ScreenShotPath, true) ){
            FString AbsoluteFilename = FPaths::ConvertRelativePathToFull(FileName);
            return currentVideoGrabber->saveTextureAsFile(AbsoluteFilename);
        }
    }
    return false;
}

// Called every frame
void UWebCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

UTexture* UWebCameraComponent::GetTexture() {
	if ( currentVideoGrabber.IsValid()) {
		return currentVideoGrabber->getTexture();
	}
	return nullptr;
}

UTexture* UWebCameraComponent::GetTextureRemote() {
#if PLATFORM_ANDROID
    __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "Called Get Remote Texture");
#endif
    if ( remoteVideoGrabber.IsValid()) {
        return remoteVideoGrabber->getTexture();
    }
    
#if PLATFORM_ANDROID
    __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "Remote Texture is invalid");
#endif
    return nullptr;
}

void UWebCameraComponent::AfterTriggerFunction(){
#if PLATFORM_ANDROID
    __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "Called After Trigger");
#endif
}



void  UWebCameraComponent::SetDeviceId(int id) {
	if (currentVideoGrabber.IsValid()) {
		if ( currentVideoGrabber->getDeviceID() == id) {
			return;
		}
		VideoGrabberPool::ReleaseVideoGrabber(currentVideoGrabber);
	}
	DeviceId.selectedDevice = id;
	currentVideoGrabber = VideoGrabberPool::GetVideoGrabber(DeviceId.selectedDevice, requestedWidth, requestedHeight, MirroredVideo);
}

TArray<FString>  UWebCameraComponent::ListDevices() {
	VideoGrabber videoGrabber;
	TArray<FString>  devices;
	TArray<FVideoDevice>  _devices = videoGrabber.listDevices();
	for ( int i = 0; i < _devices.Num(); i++) {
		devices.Add(_devices[i].deviceName);
	}
	return devices;
}

int UWebCameraComponent::GetFrontCameraId() {
	if (currentVideoGrabber.IsValid()) {
		return currentVideoGrabber->getFrontCamera();
	}

	return 0;
}

int UWebCameraComponent::GetBackCameraId() {
	if (currentVideoGrabber.IsValid()) {
		return currentVideoGrabber->getBackCamera();
	}
	return 0;
}


bool UWebCameraComponent::SwitchFrontAndBackCamera() {
    if (currentVideoGrabber.IsValid()) {
        return currentVideoGrabber->switchBackAndFront();
    }
    return false;
}
