//
//  BEAssetsManager.c
//  Sapindus
//
//  Created by Sun YuLi on 16/4/30.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#include <pthread.h>
#endif
#include "BEAssetsManager.h"
#include "MCString.h"
#ifdef __ANDROID__
static AAssetManager* assetManager_ = null;
static ANativeWindow* window_ = null;

//File
void MCFileSetAssetManager(AAssetManager* assetManager)
{
	assetManager_ = assetManager;
}

AAssetManager* MCFileGetAssetManager()
{
    return assetManager_;
}
#endif

int MCFileGetPath(const char* filename, char* buffer)
{
    return MCFileGetPathFromBundle(NULL, filename, buffer);
}

int MCFileGetPathFromBundle(const char* bundlename, const char* filename, char* buffer)
{
    char basename[256] = {0};
    char extension[64] = {0};
    
    MCString_extensionFromFilename(filename, basename, extension);
    printf("MCFileGetPath - filename/basename/extension -> %s/%s/%s\n", filename, basename, extension);
    
#ifdef __ANDROID__
    if (assetManager_ != null) {
        const char* subpath;
        if (strcmp(extension, "fsh") == 0) {
            subpath = "shaders";
        } else if (strcmp(extension, "vsh") == 0) {
            subpath = "shaders";
        } else if (strcmp(extension, "obj") == 0 || strcmp(extension, "mtl") == 0) {
            subpath = "raw";
        } else if (strcmp(extension, "png") == 0) {
            subpath = "textures";
        } else if (strcmp(extension, "jpg") == 0) {
            subpath = "textures";
        } else if (strcmp(extension, "tga") == 0) {
            subpath = "textures";
        } else {
            subpath = "raw";
            error_log("can not detect use raw folder\n");
        }
        
        AAssetDir* rootdir = AAssetManager_openDir(assetManager_, subpath);
        if (rootdir) {
            const char* name;
            char fullpath[PATH_MAX] = {0};
            while ((name=AAssetDir_getNextFileName(rootdir)) != NULL) {
                if (strcmp(filename, name) == 0) {
                    sprintf(fullpath, "%s/%s", subpath, name);
                    strcpy(buffer, fullpath);
                }
            }
        }else{
            error_log("can not find rootdir\n");
            return -1;
        }
    }
    return 0;
#else

    static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&lock);
    
    CFBundleRef bundle = NULL;
    if (bundlename) {
        CFStringRef bid = CFStringCreateWithCString(kCFAllocatorDefault, bundlename, kCFStringEncodingUTF8);
        bundle = CFBundleGetBundleWithIdentifier(bid);
        CFRelease(bid);
    } else {
        bundle = CFBundleGetMainBundle();
    }
    
    if (!bundle) {
        error_log("BEAssetManager can not find bundle (%s)\n", bundlename);
        return -1;
    }
    
    CFStringRef fname = CFStringCreateWithCString(kCFAllocatorDefault, basename, kCFStringEncodingUTF8);
    CFStringRef  fext = CFStringCreateWithCString(kCFAllocatorDefault, extension, kCFStringEncodingUTF8);
    CFURLRef url = CFBundleCopyResourceURL(bundle, fname, fext, NULL);
    if (url) {
        CFStringRef  path = CFURLCopyPath(url);
        CFStringGetCString(path, buffer, PATH_MAX, kCFStringEncodingUTF8);
        if(path) CFRelease(path);
        if(url)  CFRelease(url);
        if(fname)CFRelease(fname);
        if(fext) CFRelease(fext);
        pthread_mutex_unlock(&lock);
        return 0;
        
    } else {
        error_log("BEAssetManager can not find path of (%s).(%s)\n", basename, extension);
        if(fname)CFRelease(fname);
        if(fext) CFRelease(fext);
        pthread_mutex_unlock(&lock);
        return -1;
    }
    
#endif
}

const char* MCFileCopyContentWithPath(const char* filepath)
{
#ifdef __ANDROID__
    if (assetManager_ != null) {
        debug_log("MCFileCopyContentWithPath %s\n", filepath);
        AAsset* f = AAssetManager_open(assetManager_, filepath, AASSET_MODE_BUFFER);
        if (f) {
            const char* abuff = AAsset_getBuffer(f);
            if (abuff) {
                off_t size = AAsset_getLength(f);
                char* buff = (char*)malloc((size + 1) * sizeof(char));
                memcpy(buff, abuff, size);
                buff[size] = NUL;
                AAsset_close(f);
                return buff;
            }else{
                error_log("MCFileCopyContentWithPath(%s) AAsset_getBuffer() failed\n", filepath);
            }
        }else{
            error_log("MCFileCopyContentWithPath(%s) Android assetManager_ can not open\n", filepath);
        }
    }
    error_log("MCFileCopyContent(%s) Android assetManager_ is null\n", filepath);
    return null;
#else
    char decodepath[PATH_MAX] = {0};
    FILE* f = fopen(MCString_percentDecode(filepath, decodepath), "r");
    if (f) {
        fseek(f, 0, SEEK_END);
        long size = ftell(f);
        fseek(f, 0, SEEK_SET);
        char* buffer = (char*)malloc(size);
        if (!buffer) {
            error_log("MCFileCopyContent(%s) can not alloc buffer\n", filepath);
            return null;
        }
        memset(buffer, 0, size);
        //copy
        char* iter = buffer;
        if (f != NULL) {
            char c;
            while ((c = fgetc(f)) != EOF) {
                (*iter) = c;
                iter++;
            }
            buffer[size] = NUL;
            //*iter = NUL;
        }
        fclose(f);
        if (buffer[0] != '#') {
            
        }
        return buffer;
    }else{
        error_log("MCFileCopyContent(%s) fopen return null\n", filepath);
        return null;
    }

#endif
}

void MCFileReleaseContent(void* buff)
{
	free(buff);
}
