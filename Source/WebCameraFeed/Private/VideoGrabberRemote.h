//
//  VideoGrabberRemote.h
//  SariskaUnrealDemo
//
//  Created by Dipak Sisodiya on 14/06/22.
//  Copyright © 2022 Epic Games, Inc. All rights reserved.
//

#pragma once

#if PLATFORM_WINDOWS
    #include "DirectShowVideoGrabber.h"
    typedef DirectShowVideoGrabber VideoGrabberRemote;
#endif

#if PLATFORM_MAC
    #include "AVFoundationVideoGrabber.h"
    typedef AVFoundationVideoGrabber VideoGrabberRemote;
#endif

#if PLATFORM_IOS
    #include "AVFoundationVideoGrabber.h"
    typedef AVFoundationVideoGrabber VideoGrabberRemote;
#endif

#if PLATFORM_ANDROID
    #include "AndroidRemoteVideo.h"
    typedef AndroidRemoteVideo VideoGrabberRemote;
#endif
