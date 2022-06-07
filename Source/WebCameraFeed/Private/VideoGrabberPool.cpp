// Fill out your copyright notice in the Description page of Project Settings.

#include "VideoGrabberPool.h"

VideoGrabberPool* VideoGrabberPool::Instance = nullptr;

 VideoGrabberPool* const  VideoGrabberPool::GetInstance() {
 	if (Instance == nullptr)
	{
		Instance = new VideoGrabberPool();
	}
	return Instance;
 }

 void  VideoGrabberPool::ReleaseInstance() {
 	if ( Instance != nullptr) {
		delete Instance;
		Instance = nullptr;
	}
 }

 VideoGrabberPool::VideoGrabberPool() {
	 
 }
 VideoGrabberPool::~VideoGrabberPool() {
	 frwLock.WriteLock();
	 videoGrabbers.Empty();
	 videoGrabberReferences.Empty();
	 frwLock.WriteUnlock();
 }

TSharedPtr<VideoGrabber>  VideoGrabberPool::GetVideoGrabber ( int device, int widh, int height, bool mirrored ) {
	return GetInstance()->GetVideoGrabberInternal(device, widh, height, mirrored);
}


TSharedPtr<VideoGrabber> VideoGrabberPool::GetVideoGrabberInternal ( int device, int widh, int height, bool mirrored ) {
	frwLock.ReadLock();
	if ( videoGrabbers.Contains(device) ) {
		frwLock.ReadUnlock();	
		frwLock.WriteLock();
		videoGrabberReferences[device]++;
		frwLock.WriteUnlock();
		return videoGrabbers[device];
	} else {
		frwLock.ReadUnlock();
		frwLock.WriteLock();
		TSharedPtr<VideoGrabber>  videoGrabber ( new VideoGrabber());
		videoGrabber->setDeviceID(device);
		if ( videoGrabber->setup(widh, height, mirrored) ) {
			videoGrabbers.Add(device, videoGrabber);
			videoGrabberReferences.Add(device, 1);
		} else {
			videoGrabber = nullptr;
		}
		frwLock.WriteUnlock();
		return videoGrabber;
	}
}

void  VideoGrabberPool::ReleaseVideoGrabber(TSharedPtr<VideoGrabber> videoGrabber) {
	GetInstance()->ReleaseVideoGrabberInternal(videoGrabber);
 }

void VideoGrabberPool::ReleaseVideoGrabberInternal(TSharedPtr<VideoGrabber> videoGrabber) {
	frwLock.WriteLock();	
	if (videoGrabbers.Contains(videoGrabber->getDeviceID())) {
		
		videoGrabberReferences[videoGrabber->getDeviceID()]--;
		if ( videoGrabberReferences[videoGrabber->getDeviceID()] == 0) {
            videoGrabber->close();
			videoGrabbers.Remove(videoGrabber->getDeviceID());
			videoGrabberReferences.Remove(videoGrabber->getDeviceID());
		}
	}
	frwLock.WriteUnlock();
 }
