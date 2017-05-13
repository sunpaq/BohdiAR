//
//  BERunLoop.h
//  Pods
//
//  Created by Sun YuLi on 2017/4/22.
//
//

#import <Foundation/Foundation.h>
#import <GLKit/GLKit.h>

@interface BERunLoop : NSObject

-(instancetype) initWithTarget:(id)target Selector:(SEL)selector;
-(void) startRunloop;

@end
