//
//  BEARViewController.swift
//  BohdiAR
//
//  Created by Sun YuLi on 2017/5/13.
//  Copyright © 2017年 sunpaq. All rights reserved.
//

import UIKit

class BEARViewController: BARBaseController, BARDelegate {
    var renderer: BERenderer!
    var glview: GLKView!
    var detectedMarkers: [Int32]!
    
    override var prefersStatusBarHidden: Bool {
        return true
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        detectedMarkers = []
        
        glview = BERenderer.createDefaultGLView(self.view.frame)
        renderer = BERenderer.init(frame: self.view.frame,
                                   doesOpaque: false,
                                   cameraRotateMode: BECameraFixedAtOrigin)

        let path = Bundle.main.path(forResource: "calibrate", ofType: "xml")
        self.configDetector(withCameraParameters: path, markerLength: 2.0)
        self.drawDebugInfo = true
        self.openglContainer.addSubview(glview)
        self.delegate = self
    }

    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        self.startDetector()
    }
    
    //MARK BARDelegate
    func onDetectMarker(_ markerId: Int32, index: Int32) {
        if !detectedMarkers.contains(markerId) {
            if markerId < 5 {
                renderer.addModelNamed("2.obj", scale: 5.0, rotateX: Double.pi / 2.0, tag: markerId)
            }
            else {
                renderer.addModelNamed("arcanegolem.obj", scale: 5.0, rotateX: Double.pi / 2.0, tag: markerId)
            }
            detectedMarkers.append(markerId)
        }
    }
    
    func onUpdateMarker(_ markerId: Int32, index: Int32, pose mat4: UnsafeMutablePointer<Float>!) {
        renderer.updateModelTag(markerId, poseMat4F: mat4)
    }
    
    func onImageProcessDone() {
        renderer.drawFrame(onGLView: glview)
    }
}
