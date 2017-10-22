//
//  BEModelCache.m
//  Pods
//
//  Created by YuliSun on 01/06/2017.
//
//

#import "BEModelCache.h"
#import "MC3DModel.h"
#import "MC3DModelCache.h"

@interface BEModelCache ()
{
    MC3DModelCache* cache;
}
@end

@implementation BEModelCache

static BEModelCache* _instance = NULL;

+(instancetype)shared
{
    if (!_instance) {
        _instance = [[BEModelCache alloc] init];
        _instance->cache = MC3DModelCache_shared(0);
    }
    return _instance;
}

-(void) loadModelNamed:(NSString*)name
{
    const char* cname = [name cStringUsingEncoding:NSUTF8StringEncoding];
    if (cname) {
        MC3DModelCache_loadModelNamed(cache, cname);
    }
}

@end
