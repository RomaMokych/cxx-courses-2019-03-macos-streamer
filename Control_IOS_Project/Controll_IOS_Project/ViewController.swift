//
//  ViewController.swift
//  Controll_IOS_Project
//
//  Created by MacOS on 28.03.2019.
//  Copyright © 2019 MacOS. All rights reserved.
//

import UIKit

class ViewController: UIViewController, UITextFieldDelegate {

    @IBOutlet weak var keyboardStreamTextField: UITextField!
    
    @IBOutlet weak var videoView: UIView!
    
    @IBAction func disconnect(_ sender: UIButton) {
        //disconnect
        print("disconnect")
    }
    
    
    
    var buffer = UnsafeMutablePointer<UInt8>.allocate(capacity: 18)
    
    var receiver = MessageReceiver()
    
    
    override func viewDidLoad() {
        super.viewDidLoad()
        keyboardStreamTextField.delegate = self
        
        self.view.isMultipleTouchEnabled = true
        //Gesture Recognizer
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
        
        
        receiver.controller = self
        receiver.setupNetworkCommunication();
        
        videoView.isOpaque = true
        videoView.layer.rasterizationScale = UIScreen.main.scale;
        videoView.layer.shouldRasterize = false
    }
    
    func toByteArray<T>(value: T) -> [UInt8]
    {
        var val = value
        return withUnsafeBytes(of: &val) {Array($0)}
    
    }
    
    func fromByteArray<T>(value: [UInt8], _: T.Type) -> T
    {
        return value.withUnsafeBytes { $0.baseAddress!.load(as: T.self) }
    }
    
    @objc func mouseMove(recognizer: UIPanGestureRecognizer){
        
        if recognizer.state == .began{
            
        } else if recognizer.state == .changed {
            let point = recognizer.translation(in: videoView)
            
            
            let x : CGFloat = point.x;
            let y : CGFloat = point.y;
            
            var bytesX = toByteArray(value : x)
            var bytesY = toByteArray(value : y)
            
            
            buffer[0] = 1; // Start
            
            buffer[1] = bytesX[0];
            buffer[2] = bytesX[1];
            buffer[3] = bytesX[2];
            buffer[4] = bytesX[3];
            
            buffer[5] = bytesX[4];
            buffer[6] = bytesX[5];
            buffer[7] = bytesX[6];
            buffer[8] = bytesX[7];
            
            buffer[9] =  bytesY[0];
            buffer[10] = bytesY[1];
            buffer[11] = bytesY[2];
            buffer[12] = bytesY[3];
            
            buffer[13] = bytesY[4];
            buffer[14] = bytesY[5];
            buffer[15] = bytesY[6];
            buffer[16] = bytesY[7];
            
            buffer[17] = 1; // End
            
            
            receiver.outputStream.write(buffer, maxLength: 18)
            
            print("Moving", point)
            
            
            recognizer.setTranslation(CGPoint.zero, in: videoView)
            
        } else if recognizer.state == .ended {
            
        }
        
    }
    @objc func mouseDownMove(recognizer: UIPanGestureRecognizer){
        
        if recognizer.state == .began{
            print("mouse down")//transfer to server
            
        } else if recognizer.state == .changed {
            let point = recognizer.translation(in: videoView)//transfer to server
            print("Moving and mouse down", point)
            recognizer.setTranslation(CGPoint.zero, in: videoView)
            
        } else if recognizer.state == .ended {
           print("Mouse up")//transfer to server
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
            let point = recognizer.translation(in: videoView) //transfer to server
            if point.y > 0{
                print("scrollDown", point.y)
            } else if point.y < 0 {
                print("scrollUp",point.y)
            }
            recognizer.setTranslation(CGPoint.zero, in: videoView)
            
        } else if recognizer.state == .ended {
            
        }
        
    }
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        print(keyboardStreamTextField.text!)    //transfer to server
        keyboardStreamTextField.text = ""       //clear text
        return true
    }
}

