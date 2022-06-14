// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VideoGrabber.h"
#include "VideoGrabberRemote.h"

class VideoGrabberPool
{
public:

	static VideoGrabberPool* const GetInstance();
	static void ReleaseInstance();

	static TSharedPtr<VideoGrabber> GetVideoGrabber ( int device, int widh, int height, bool mirrored  );
    
    static TSharedPtr<VideoGrabberRemote> GetVideoGrabberRemote ( int device, int widh, int height, bool mirrored  );

	static void ReleaseVideoGrabber(TSharedPtr<VideoGrabber> videoGrabber);
    
    static void ReleaseVideoGrabberRemote(TSharedPtr<VideoGrabberRemote> videoGrabberRemote);

private:

	static VideoGrabberPool* Instance;

	VideoGrabberPool();
	~VideoGrabberPool();

	TSharedPtr<VideoGrabber> GetVideoGrabberInternal ( int device, int widh, int height, bool mirrored );
    TSharedPtr<VideoGrabberRemote> GetVideoGrabberInternalRemote ( int device, int widh, int height, bool mirrored );
    
	void ReleaseVideoGrabberInternal(TSharedPtr<VideoGrabber> videoGrabber);
    void ReleaseVideoGrabberInternalRemote(TSharedPtr<VideoGrabberRemote> videoGrabberRemote);
    

	TMap<int, TSharedPtr<VideoGrabber>> videoGrabbers;
    TMap<int, TSharedPtr<VideoGrabberRemote>> videoGrabbersRemote;
    
	TMap<int, int>			videoGrabberReferences;

	FRWLock	 frwLock;
};

