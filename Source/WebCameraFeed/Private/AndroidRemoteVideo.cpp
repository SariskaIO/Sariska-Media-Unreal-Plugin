//
//  AndroidRemoteVideo.cpp
//  SariskaUnrealDemo_Index
//
//  Created by Dipak Sisodiya on 14/06/22.
//  Copyright © 2022 Epic Games, Inc. All rights reserved.
//

#include "AndroidRemoteVideo.h"
#include <Async/Async.h>
#if PLATFORM_ANDROID
#include "Android/AndroidApplication.h"
#include <Runtime/Launch/Public/Android/AndroidJNI.h>
#include <android/log.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#define LOG_TAG "CameraLOG"

static long int crv_tab_Remote[256];
static long int cbu_tab_Remote[256];
static long int cgu_tab_Remote[256];
static long int cgv_tab_Remote[256];
static long int tab_76309_Remote[256];
static unsigned char clp_Remote[1024];

static AndroidRemoteVideo* currentGrabberRemote = NULL;

void InitConvertTable_Remote()
{
    static bool inited = false;
    if (inited) return;
    long int crv, cbu, cgu, cgv;
    int i, ind;

    crv = 104597; cbu = 132201;  /* fra matrise i global.h */
    cgu = 25675;  cgv = 53279;

    for (i = 0; i < 256; i++) {
        crv_tab_Remote[i] = (i - 128) * crv;
        cbu_tab_Remote[i] = (i - 128) * cbu;
        cgu_tab_Remote[i] = (i - 128) * cgu;
        cgv_tab_Remote[i] = (i - 128) * cgv;
        tab_76309_Remote[i] = 76309 * (i - 16);
    }

    for (i = 0; i < 384; i++)
        clp_Remote[i] = 0;
    ind = 384;
    for (i = 0; i < 256; i++)
        clp_Remote[ind++] = i;
    ind = 640;
    for (i = 0; i < 384; i++)
        clp_Remote[ind++] = 255;

    inited = true;
}

TArray<FVideoDevice> AndroidRemoteVideo::listDevices() const {
    TArray<FVideoDevice> devices;
    return devices;
}

bool AndroidRemoteVideo::isFrameNew() const {
    return newFrame;
}

int AndroidRemoteVideo::getHeight() const {
    return height;
}

int AndroidRemoteVideo::getWidth() const {
    return width;
}

bool  AndroidRemoteVideo::switchBackAndFront() {
    return true;
}

void AndroidRemoteVideo::pause() {
    // Do nothing
}
   
void  AndroidRemoteVideo::resume() {
   //Do nothing
}

int  AndroidRemoteVideo::getBackCamera() const {
    return 1;
}

int  AndroidRemoteVideo::getFrontCamera() const {
    return 2;
}


void ConvertYUV2RGBA_Remote(unsigned char *src0, unsigned char *src1, unsigned char *dst_ori,
    int width, int height, bool mirror)
{
    register int y1, y2, u, v;
    register unsigned char *py1, *py2;
    register int i, j, c1, c2, c3, c4;
    register unsigned char *d1, *d2;

    int width4 = 4 * width;
    py1 = src0;
    py2 = py1 + width;
    if ( mirror ) {
        d1 = dst_ori + width4 - 1;
        d2 = d1 + width4;
    } else {
        d1 = dst_ori;
        d2 = d1 + width4;
    }
    for (j = 0; j < height; j += 2) {
        for (i = 0; i < width; i += 2) {

            v = *src1++;
            u = *src1++;

            c1 = crv_tab_Remote[v];
            c2 = cgu_tab_Remote[u];
            c3 = cgv_tab_Remote[v];
            c4 = cbu_tab_Remote[u];

            if ( mirror ) {
                //up-left
                y1 = tab_76309_Remote[*py1++];
                *d1-- = 255;
                *d1-- = clp_Remote[384 + ((y1 + c4) >> 16)];
                *d1-- = clp_Remote[384 + ((y1 - c2 - c3) >> 16)];
                *d1-- = clp_Remote[384 + ((y1 + c1) >> 16)];
                
                //down-left
                y2 = tab_76309_Remote[*py2++];
                *d2-- = 255;
                *d2-- = clp_Remote[384 + ((y2 + c4) >> 16)];
                *d2-- = clp_Remote[384 + ((y2 - c2 - c3) >> 16)];
                *d2-- = clp_Remote[384 + ((y2 + c1) >> 16)];

                //up-right
                y1 = tab_76309_Remote[*py1++];
                *d1-- = 255;
                *d1-- = clp_Remote[384 + ((y1 + c4) >> 16)];
                *d1-- = clp_Remote[384 + ((y1 - c2 - c3) >> 16)];
                *d1-- = clp_Remote[384 + ((y1 + c1) >> 16)];
        
                //down-right
                y2 = tab_76309_Remote[*py2++];
                *d2-- = 255;
                *d2-- = clp_Remote[384 + ((y2 + c4) >> 16)];
                *d2-- = clp_Remote[384 + ((y2 - c2 - c3) >> 16)];
                *d2-- = clp_Remote[384 + ((y2 + c1) >> 16)];
            } else {
                //up-left
                y1 = tab_76309_Remote[*py1++];
                *d1++ = clp_Remote[384 + ((y1 + c1) >> 16)];
                *d1++ = clp_Remote[384 + ((y1 - c2 - c3) >> 16)];
                *d1++ = clp_Remote[384 + ((y1 + c4) >> 16)];
                *d1++ = 255;

                //down-left
                y2 = tab_76309_Remote[*py2++];
                *d2++ = clp_Remote[384 + ((y2 + c1) >> 16)];
                *d2++ = clp_Remote[384 + ((y2 - c2 - c3) >> 16)];
                *d2++ = clp_Remote[384 + ((y2 + c4) >> 16)];
                *d2++ = 255;

                //up-right
                y1 = tab_76309_Remote[*py1++];
                *d1++ = clp_Remote[384 + ((y1 + c1) >> 16)];
                *d1++ = clp_Remote[384 + ((y1 - c2 - c3) >> 16)];
                *d1++ = clp_Remote[384 + ((y1 + c4) >> 16)];
                *d1++ = 255;

                //down-right
                y2 = tab_76309_Remote[*py2++];
                *d2++ = clp_Remote[384 + ((y2 + c1) >> 16)];
                *d2++ = clp_Remote[384 + ((y2 - c2 - c3) >> 16)];
                *d2++ = clp_Remote[384 + ((y2 + c4) >> 16)];
                *d2++ = 255;
            }
        }
        if ( mirror) {
            d1 = dst_ori + (j+1)*width4 - 1;
            d2 = d1 + width4;
        } else {
            d1 += width4;
            d2 += width4;
        }
        py1 += width;
        py2 += width;
    }


}


AndroidRemoteVideo::AndroidRemoteVideo() {
    InitConvertTable_Remote();
    fps = -1;
    width = 0;
    height = 0;
    bIsInit = false;
    newFrame = false;
    bHavePixelsChanged = false;
    textureID = 0;
    _mirror = false;
}

AndroidRemoteVideo:: ~AndroidRemoteVideo() {
    close();
}


void AndroidRemoteVideo::loadTexture() {
    
    GLuint TextureID = 0;
    glGenTextures(1, &TextureID);

    glEnable(GL_TEXTURE_EXTERNAL_OES);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, TextureID);

    glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    

    glDisable(GL_TEXTURE_EXTERNAL_OES);

    textureID = TextureID;
    
}

void AndroidRemoteVideo::deleteTexture () {
    if ( textureID != 0) {
        GLuint TextureID = TextureID;
        glDeleteTextures(1, &TextureID);
        textureID = 0;
    }
}

void AndroidRemoteVideo::setDeviceID(int deviceID) {

    this->deviceID = deviceID;
}


bool  AndroidRemoteVideo::setup(int w, int h, bool mirrored) {
    __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "Called setup");
    loadTexture();

    
        setVideoMirrored(false);
        _mirror = mirrored;
        width = w;
        height = h;
        allocateData(width, height, PF_B8G8R8A8);
        startThread();
        bIsInit = true;
        registerDelegates();
        currentGrabberRemote = this;
    
    return true;
}

void  AndroidRemoteVideo::close() {
    currentGrabberRemote = NULL;
    stopThread();
    deleteTexture();
    unRegisterDelegates();
    bIsInit = false;
    width = 0;
    height = 0;
    fps = -1;
    newFrame = false;
    bHavePixelsChanged = false;
}

void  AndroidRemoteVideo::update() {

    if ( !bIsInit) return;

    newFrame = false;

    if ( currentGrabberRemote == NULL) {
        currentGrabberRemote = this;
    }

    if (bHavePixelsChanged ) {
        bHavePixelsChanged = false;
        frwLock.WriteLock();

        if (cameraTexture.IsValid()) {
            FUpdateTextureRegion2D region(0, 0, 0, 0, width, height);
            cameraTexture->UpdateTextureRegions(0, 1, &region, width*4, 4, pixels.GetData());
        }
        frwLock.WriteUnlock();
        newFrame = true;
    }
}

void AndroidRemoteVideo::updatePixelsCB(unsigned char *isrc, int w, int h) {
    if (!bIsInit) return;
    
    if (w != width || h != height || !cameraTexture.IsValid()) {
        if (!cameraTexture.IsValid()) {
            UE_LOG(LogVideoGrabber, Warning, TEXT("The texture is invalid, reallocating"));
            __android_log_print(ANDROID_LOG_WARN, LOG_TAG, "The texture is invalid, reallocating");
        }
        else {
            UE_LOG(LogVideoGrabber, Warning, TEXT("The incoming image dimensions %d by %d don't match with the current dimensions %d by %d"), w, h, width, height);
            __android_log_print(ANDROID_LOG_WARN, LOG_TAG, "The incoming image dimensions %d by %d don't match with the current dimensions %d by %d", w, h, width, height );
        }

        FEvent* fSemaphore = FGenericPlatformProcess::GetSynchEventFromPool(false);
        AsyncTask(ENamedThreads::GameThread, [this, w, h, fSemaphore]() {
            this->resizeData(w, h, PF_B8G8R8A8);
            __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Resized data");
            fSemaphore->Trigger();
        });

        fSemaphore->Wait();
        FGenericPlatformProcess::ReturnSynchEventToPool(fSemaphore);
        width = w;
        height = h;

    }
    frwLock.ReadLock();
    if (pixels.Num() > 0) {
        
        if ( copyPixels.Num() != pixels.Num()) {
            copyPixels.Reset(pixels.Num());
        }
        frwLock.ReadUnlock();
    
        ConvertYUV2RGBA_Remote(isrc, // y component
            isrc + (w * h),  // uv components
            copyPixels.GetData(), w, h, _mirror);

        frwLock.WriteLock();
        uint32 MemorySize = w*h*4;
        FMemory::Memcpy(pixels.GetData(), copyPixels.GetData(), MemorySize);
        bHavePixelsChanged = true;
        frwLock.WriteUnlock();
    } else {
        frwLock.ReadUnlock();
    }
    
}


int SetupJNICamera_Remote(JNIEnv* env)
{
    return JNI_OK;
}



extern "C" bool Java_com_epicgames_ue4_GameActivity_nativeGetFrameDataRemote(JNIEnv* LocalJNIEnv, jobject LocalThiz, jint frameWidth, jint frameHeight, jbyteArray data) {
    __android_log_print(ANDROID_LOG_WARN, LOG_TAG, "Called Native Get Frame Remote");
    if ( currentGrabberRemote != NULL) {
        //get the new frame
        int length = LocalJNIEnv->GetArrayLength(data);
        jboolean isCopy;
        __android_log_print(ANDROID_LOG_WARN, LOG_TAG, "currentGrabberIs not null");
        auto currentFrame = (unsigned char *)LocalJNIEnv->GetByteArrayElements(data, &isCopy);
        if (currentFrame) {
            currentGrabberRemote->updatePixelsCB(currentFrame, frameWidth, frameHeight);
            LocalJNIEnv->ReleaseByteArrayElements(data, (jbyte*)currentFrame, 0);
        }
    }
    __android_log_print(ANDROID_LOG_WARN, LOG_TAG, "currentGrabberIs null");
    return JNI_TRUE;
}


#endif
