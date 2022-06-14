//
//  AndroidVideoRemote.cpp
//  SariskaUnrealDemo_Index
//
//  Created by Dipak Sisodiya on 12/06/22.
//  Copyright © 2022 Epic Games, Inc. All rights reserved.
//

#include "AndroidVideoRemote.h"
#include <Async/Async.h>
#if PLATFORM_ANDROID
#include <Engine/Texture2D.h>
#include "Android/AndroidApplication.h"
#include <Runtime/Launch/Public/Android/AndroidJNI.h>
#include <android/log.h>
#include "WebCameraComponent.h"
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

static long int crv_tab_Remote[256];
static long int cbu_tab_Remote[256];
static long int cgu_tab_Remote[256];
static long int cgv_tab_Remote[256];
static long int tab_76309_Remote[256];
static unsigned char clp_Remote[1024];

static AndroidVideoRemote* currentGrabberRemote = NULL;
int SetupJNICamera(JNIEnv* env);
//JNIEnv* ENV = NULL;

void InitConverTable(){
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

int AndroidVideoRemote::returnNumber(){
    return 21;
}

AndroidVideoRemote* AndroidVideoRemote::get_instance(){
    return new AndroidVideoRemote();
}

void AndroidVideoRemote::allocateDataRemote(int w, int h, EPixelFormat InFormat){
    
    if(isTextureGenerated == true){
        return;
    }

    frwLock.WriteLock();
    
    uint32 MemorySize = w * h * 4;
    pixels.Reset();
    pixels.AddUninitialized(MemorySize);
    FMemory::Memzero(pixels.GetData(), MemorySize);
    
    remoteTexture = UTexture2D::CreateTransient(w, h, InFormat);
    FTexture2DMipMap& Mip = remoteTexture->PlatformData->Mips[0];
    void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
    FMemory::Memcpy(Data, pixels.GetData(), MemorySize);
    Mip.BulkData.Unlock();
    remoteTexture->UpdateResource();
    isTextureGenerated = true;
    frwLock.WriteUnlock();
}

UTexture* AndroidVideoRemote::returnTexture(){
    return remoteTexture.Get();
}

AndroidVideoRemote::AndroidVideoRemote(){
    bIsInit = false;
    number = 1;
    InitConvertTable();
    bIsInit = false;
    textureID = 0;
    _mirror = false;
}

AndroidVideoRemote:: ~AndroidVideoRemote() {
    //Do nothing for now
    // add a close function here
}

bool AndroidVideoRemote::setup(int w, int h, bool mirrored) {
    loadTexture();
    bIsInit = true;
    __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "Called setup for remote");
    currentGrabberRemote = this;
    
    //allocateData(width, height, PF_B8G8R8A8); // Part of BaseVideoGrabber
    //startThread(); // Part of BaseVideoGrabber
    //registerDelegates(); //Part of BaseVideoGrabber
    //currentGrabber = this; // Not needed for now
    return true;
}


void AndroidVideoRemote::loadTexture() {
    
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

void AndroidVideoRemote::ConvertYUV2RGBA(unsigned char *src0, unsigned char *src1, unsigned char *dst_ori,
    int width, int height, bool mirror)
{
    __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "Called YUV");
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


void AndroidVideoRemote::deleteTexture () {
    if ( textureID != 0) {
        GLuint TextureID = TextureID;
        glDeleteTextures(1, &TextureID);
        textureID = 0;
    }
}

void AndroidVideoRemote::updatePixelsCB(unsigned char *isrc, int w, int h) {
   //Do nothing for now
    ConvertYUV2RGBA(isrc, // y component
        isrc + (w * h),  // uv components
        copyPixels.GetData(), w, h, _mirror);
}

void  AndroidVideoRemote::close() {
    currentGrabberRemote = NULL;
    bIsInit = false;
    deleteTexture();
}

extern "C" bool Java_com_epicgames_ue4_GameActivity_nativeGetFrameDataRemote(JNIEnv* LocalJNIEnv, jobject LocalThiz, jint frameWidth, jint frameHeight, jbyteArray data){
    __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "Called from java");
    int length = LocalJNIEnv->GetArrayLength(data);
    jboolean isCopy;
    auto currentFrame = (unsigned char*)LocalJNIEnv->GetByteArrayElements(data, &isCopy);
    if(currentFrame){
        currentGrabberRemote->updatePixelsCB(currentFrame, frameWidth, frameHeight);
        LocalJNIEnv->ReleaseByteArrayElements(data, (jbyte*)currentFrame, 0);
    }
    
    return JNI_TRUE;
}


#endif
