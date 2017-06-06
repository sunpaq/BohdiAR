#ifdef __OBJC__
#import <UIKit/UIKit.h>
#else
#ifndef FOUNDATION_EXPORT
#if defined(__cplusplus)
#define FOUNDATION_EXPORT extern "C"
#else
#define FOUNDATION_EXPORT extern
#endif
#endif
#endif

#import "BARView.h"
#import "BARAbstractCamera.hpp"
#import "BARPhotoCamera.hpp"
#import "BARVideoCamera.hpp"

FOUNDATION_EXPORT double BohdiARVersionNumber;
FOUNDATION_EXPORT const unsigned char BohdiARVersionString[];

