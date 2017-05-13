//
//  MainViewController.swift
//  BohdiAR
//
//  Created by Sun YuLi on 2017/5/13.
//  Copyright © 2017年 sunpaq. All rights reserved.
//

import UIKit

class MainViewController: UIViewController, UIDocumentInteractionControllerDelegate {

    //MARK pdf document
    var docmanager: UIDocumentInteractionController!
    var doclist: [String] = ["ARUCO_MIP_36h12_test"]
    
    func setupDocManager() {
        docmanager = UIDocumentInteractionController()
        docmanager.delegate = self
    }
    
    func documentInteractionControllerViewControllerForPreview(_ controller: UIDocumentInteractionController) -> UIViewController {
        return self
    }
    
    @IBAction func showMarkersAction(_ sender: Any) {
         let url = Bundle.main.url(forResource: doclist[0], withExtension: "pdf")
         docmanager.url = url
         docmanager.presentPreview(animated: true)
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        setupDocManager()
    }
}
