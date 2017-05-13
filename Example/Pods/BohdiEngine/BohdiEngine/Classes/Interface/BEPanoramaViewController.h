//
//  SphereViewController.h
//  BEDemo
//
//  Created by YuliSun on 24/03/2017.
//  Copyright © 2017 SODEC. All rights reserved.
//


#import <UIKit/UIKit.h>
#import <BohdiEngine/BEViewController.h>

@interface BEPanoramaViewController : UIViewController <BEViewDelegate>

@property (atomic, readwrite) NSString* textureFileName;

-(IBAction)onClose:(id)sender;

@end
