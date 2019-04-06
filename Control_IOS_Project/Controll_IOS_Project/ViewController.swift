//
//  ViewController.swift
//  Controll_IOS_Project
//
//  Created by MacOS on 28.03.2019.
//  Copyright © 2019 MacOS. All rights reserved.
//

import UIKit

class ViewController: UIViewController {

    @IBOutlet weak var videoView: UIView!
    
    @IBOutlet weak var keyboardTest: UITextField!
    override func viewDidLoad() {
        super.viewDidLoad()
        self.view.isMultipleTouchEnabled = true
        
        let panGestureRecognizer = UIPanGestureRecognizer(target: self, action: #selector(mouseMove))
        let panTwoTouchesGestureRecognizer = UIPanGestureRecognizer(target: self, action: #selector(mouseDownMove))
        panTwoTouchesGestureRecognizer.minimumNumberOfTouches = 2
        panTwoTouchesGestureRecognizer.maximumNumberOfTouches = 2
        let panThreeTouchesGestureRecognizer = UIPanGestureRecognizer(target: self, action: #selector(scroll))
        panThreeTouchesGestureRecognizer.minimumNumberOfTouches = 3
        panThreeTouchesGestureRecognizer.maximumNumberOfTouches = 3
        let leftTapGestureRecognizer = UITapGestureRecognizer(target: self, action: #selector(leftTap))
        let rightTapGestureRecognizer = UITapGestureRecognizer(target: self, action: #selector(rightTap))
        rightTapGestureRecognizer.numberOfTouchesRequired = 2
        
        
        
        
        videoView.addGestureRecognizer(panGestureRecognizer)
        videoView.addGestureRecognizer(panTwoTouchesGestureRecognizer)
        videoView.addGestureRecognizer(panThreeTouchesGestureRecognizer)
        videoView.addGestureRecognizer(leftTapGestureRecognizer)
        videoView.addGestureRecognizer(rightTapGestureRecognizer)
    }
    @objc func mouseMove(recognizer: UIPanGestureRecognizer){
        
        if recognizer.state == .began{
            
        } else if recognizer.state == .changed {
            let point = recognizer.translation(in: videoView)
            print("Moving", point)
            recognizer.setTranslation(CGPoint.zero, in: videoView)
            
        } else if recognizer.state == .ended {
            
        }
        
    }
    @objc func mouseDownMove(recognizer: UIPanGestureRecognizer){
        
        if recognizer.state == .began{
            print("mouse down")
            
        } else if recognizer.state == .changed {
            let point = recognizer.translation(in: videoView)
            print("Moving and mouse down", point)
            recognizer.setTranslation(CGPoint.zero, in: videoView)
            
        } else if recognizer.state == .ended {
           print("Mouse up")
        }
        
    }
    @objc func leftTap(recognizer: UITapGestureRecognizer){
 
        if recognizer.state == .ended {
        print("just left tap",recognizer.location(in: videoView))
        }
    }
    @objc func rightTap(recognize: UITapGestureRecognizer){
        if recognize.state == .ended{
            print("jusr right tap", recognize.location(in: videoView))
        }
    }
    @objc func scroll(recognizer: UIPanGestureRecognizer){
        
        if recognizer.state == .began{
            
        } else if recognizer.state == .changed {
            let point = recognizer.translation(in: videoView)
            if point.y > 0{
                print("scrollDown", point.y)
            } else if point.y < 0 {
                print("scrollUp",point.y)
            }
            recognizer.setTranslation(CGPoint.zero, in: videoView)
            
        } else if recognizer.state == .ended {
            
        }
        
    }
}

