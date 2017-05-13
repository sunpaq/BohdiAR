//
//  SphereViewController.m
//  BEDemo
//
//  Created by YuliSun on 24/03/2017.
//  Copyright © 2017 SODEC. All rights reserved.
//

#import "BEPanoramaViewController.h"

@interface BEPanoramaViewController ()
{
    BEViewController* bec;
}
@end

@implementation BEPanoramaViewController

@synthesize textureFileName;

- (void)viewDidLoad {
    [super viewDidLoad];
    bec = [[BEViewController alloc] init];
    bec.useTransparentBackground = YES;
    bec.useMultisampleAntiAlias  = YES;
    bec.touchDelegate = self;
    [bec glviewResize:self.view.frame];
}

-(void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    [self becomeFirstResponder];
    
    if (textureFileName && textureFileName.length > 0) {
        //have valid sky sphere texture name
        [self presentViewController:bec animated:YES completion:^{
            bec.cameraRotateMode = BECameraRotateAroundModelByGyroscope;
            [bec startDeviceMotion];
            //bec.useDeltaRotationData = YES;
            [bec addSkysphNamed:textureFileName];
        }];
    }
}

-(BOOL)canBecomeFirstResponder
{
    return YES;
}

-(void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
    if (bec) {
        [bec dismissViewControllerAnimated:NO completion:nil];
        bec = nil;
    }
}

-(void)viewDidDisappear:(BOOL)animated
{
    [self resignFirstResponder];
    [super viewDidDisappear:animated];
    if (bec) {
        [bec dismissViewControllerAnimated:NO completion:nil];
        bec = nil;
    }
}

-(void)motionEnded:(UIEventSubtype)motion withEvent:(UIEvent *)event
{
    if (motion == UIEventSubtypeMotionShake) {
        [self onClose:nil];
    }
}

-(IBAction)onClose:(id)sender
{
    [self viewWillDisappear:NO];
    [self dismissViewControllerAnimated:YES completion:nil];
}

-(void)onBETouched:(id)sender
{
    [self onClose:sender];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
