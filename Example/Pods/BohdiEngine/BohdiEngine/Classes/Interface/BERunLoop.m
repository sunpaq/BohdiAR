//
//  BERunLoop.m
//  Pods
//
//  Created by Sun YuLi on 2017/4/22.
//
//

#import "BERunLoop.h"

@interface BERunLoop()
{
    CADisplayLink* displayLink;
}
@end

@implementation BERunLoop

-(instancetype) initWithTarget:(id)target Selector:(SEL)selector
{
    if (self = [self init]) {
        displayLink = [CADisplayLink displayLinkWithTarget:target selector:selector];
        return self;
    }
    return nil;
}

-(void) startRunloop
{
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

-(void) stopRunloop
{
    [displayLink removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

@end
