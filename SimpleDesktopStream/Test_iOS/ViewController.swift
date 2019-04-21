//
//  ViewController.swift
//  Test_iOS
//
//  Created by Nikita on 3/15/19.
//  Copyright © 2019 Nikita. All rights reserved.
//

import UIKit

class ViewController: UIViewController {

    
    @IBOutlet var ImageViewer: UIImageView!
    
    var receiver = MessageReceiver()
    
    
    override func viewDidLoad() {
        super.viewDidLoad()
        receiver.controller = self
        receiver.setupNetworkCommunication();
        
        ImageViewer.isOpaque = true
        ImageViewer.layer.rasterizationScale = UIScreen.main.scale;
        ImageViewer.layer.shouldRasterize = false
        
    }
}

