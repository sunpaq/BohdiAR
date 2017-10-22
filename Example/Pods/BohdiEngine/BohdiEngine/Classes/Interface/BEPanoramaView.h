//
//  BEPanoramaView.h
//  Pods
//
//  Created by Sun YuLi on 2017/5/22.
//
//

#import "BEView.h"

@interface BEPanoramaView : BEView

- (void) loadPanoramaTexture:(NSString*)texname;
- (void) startDraw;
- (void) stopDraw;

//disable super class APIs
- (void) loadModelNamed:(NSString*)name __unavailable;
- (void) loadSkysphNamed:(NSString*)texname __unavailable;
- (void) startDraw3DContent:(BECameraRotateMode)rmode __unavailable;
- (void) stopDraw3DContent __unavailable;

@end
