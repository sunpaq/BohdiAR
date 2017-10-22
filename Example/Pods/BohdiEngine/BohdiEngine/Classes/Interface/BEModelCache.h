//
//  BEModelCache.h
//  Pods
//
//  Created by YuliSun on 01/06/2017.
//
//

#import <Foundation/Foundation.h>

@interface BEModelCache : NSObject

+(instancetype)shared;

-(void) loadModelNamed:(NSString*)name;

@end
