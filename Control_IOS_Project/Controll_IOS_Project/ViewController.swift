//
//  ViewController.swift
//  Controll_IOS_Project
//
//  Created by MacOS on 28.03.2019.
//  Copyright © 2019 MacOS. All rights reserved.
//

import UIKit

class ViewController: UIViewController, UITextFieldDelegate {
    
    @IBOutlet weak var videoView: UIView!
    //hide textField
    @IBOutlet weak var keyboardStreamTextField: UITextField!
    //off and on keyboard
    @IBAction func keyboardUpDown(_ sender: UIButton) {
        if keyboardStreamTextField.isFirstResponder{
            keyboardStreamTextField.resignFirstResponder()
        } else {
            keyboardStreamTextField.becomeFirstResponder()
        }
    }
    @IBAction func disconnect(_ sender: UIButton) {
        //disconnect
        print("disconnect")
    }
    
    
    
    var buffer = UnsafeMutablePointer<UInt8>.allocate(capacity: 18)
    
    var receiver = MessageReceiver()
    
    
    override func viewDidLoad() {
        super.viewDidLoad()
        //delegate
        keyboardStreamTextField.delegate = self
        keyboardStreamTextField.addTarget(self,action:#selector(textFieldChangeStream(_ :)), for: .allEditingEvents)
        
        self.view.isMultipleTouchEnabled = true
        
        //Listen keyboards events
        NotificationCenter.default.addObserver(self, selector: #selector(keyboardChange(notification:)), name: UIResponder.keyboardWillShowNotification , object: nil)
        NotificationCenter.default.addObserver(self, selector: #selector(keyboardChange(notification:)), name: UIResponder.keyboardWillHideNotification, object: nil)
        
        //Create Gesture Recognizer
        let panGestureRecognizer = UIPanGestureRecognizer(target: self, action: #selector(mouseMove))
        let panTwoTouchesGestureRecognizer = UIPanGestureRecognizer(target: self, action: #selector(mouseDownMove))
        panTwoTouchesGestureRecognizer.minimumNumberOfTouches = 2
        panTwoTouchesGestureRecognizer.maximumNumberOfTouches = 2
        let panThreeTouchesGestureRecognizer = UIPanGestureRecognizer(target: self, action: #selector(scroll))
        panThreeTouchesGestureRecognizer.minimumNumberOfTouches = 3
        panThreeTouchesGestureRecognizer.maximumNumberOfTouches = 3
        let leftDoubleTapGestureRecognizer = UITapGestureRecognizer(target: self, action: #selector(leftDoubleTap))
        leftDoubleTapGestureRecognizer.numberOfTapsRequired = 2
        let leftTapGestureRecognizer = UITapGestureRecognizer(target: self, action: #selector(leftTap))
        leftTapGestureRecognizer.require(toFail: leftDoubleTapGestureRecognizer)
        let rightTapGestureRecognizer = UITapGestureRecognizer(target: self, action: #selector(rightTap))
        rightTapGestureRecognizer.numberOfTouchesRequired = 2
        //add Gesture Recognizer
        videoView.addGestureRecognizer(panGestureRecognizer)
        videoView.addGestureRecognizer(panTwoTouchesGestureRecognizer)
        videoView.addGestureRecognizer(panThreeTouchesGestureRecognizer)
        videoView.addGestureRecognizer(leftTapGestureRecognizer)
        videoView.addGestureRecognizer(leftDoubleTapGestureRecognizer)
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
    //Mark: - Gesture Recognizer
    @objc func mouseMove(recognizer: UIPanGestureRecognizer){
        // code 7 and x and y
        if recognizer.state == .began{
            
        } else if recognizer.state == .changed {
            var point = recognizer.translation(in: videoView)
            
            
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
            point.x = pow(point.x, 2)*0.25
            point.y = pow(point.y, 2)*0.25
            print("Moving", point)
            
            
            recognizer.setTranslation(CGPoint.zero, in: videoView)
            
        } else if recognizer.state == .ended {
            
        }
        
    }
    @objc func mouseDownMove(recognizer: UIPanGestureRecognizer){
        
        if recognizer.state == .began{
            //code 4
            print("mouse down")//transfer to server
            
        } else if recognizer.state == .changed {
            //code 5 and x and y
            let point = recognizer.translation(in: videoView)//transfer to server
            print("Moving and mouse down", point.x,point.y)
            recognizer.setTranslation(CGPoint.zero, in: videoView)
            
        } else if recognizer.state == .ended {
            //code 6
           print("Mouse up")//transfer to server
        }
        
    }
    @objc func leftTap(recognizer: UITapGestureRecognizer){
        if recognizer.state == .ended {
            //code 1
            print("just left tap",recognizer.numberOfTapsRequired)
        }
    }
    @objc func leftDoubleTap(recognizer: UITapGestureRecognizer){
        if recognizer.state == .ended {
            //code 2
            print("just double left tap",recognizer.numberOfTapsRequired)
        }
    }
    @objc func rightTap(recognize: UITapGestureRecognizer){
        if recognize.state == .ended{
            //code 3
            print("jusr right tap", recognize.location(in: videoView))
        }
    }
    @objc func scroll(recognizer: UIPanGestureRecognizer){
        
        if recognizer.state == .began{
            //code 11 and y
        } else if recognizer.state == .changed {
            let point = recognizer.translation(in: videoView).y //transfer to server
            if point > 0{
                print("scrollDown", point)
            } else if point < 0 {
                print("scrollUp",point)
            }
            recognizer.setTranslation(CGPoint.zero, in: videoView)
            
        } else if recognizer.state == .ended {
            
        }
        
    }
    //Mark: - Text Field
    @objc func textFieldChangeStream(_ textField: UITextField){
        if (textField.text!.count > 1){
            //code 8 and popLast
            print(textField.text!.popLast()!)        //transfer to server
        } else
        if textField.text!.isEmpty {
            //code 10
            print("transfer to server code code 51")
            textField.text = "/"
        }
    }
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        //code 9
        print("transfer to server code code 36")
        textField.text = "/"                        //clear text
        return false
    }
    
    //Mark: - Notification
    //Change frame view when call keyboard
    @objc func keyboardChange(notification: Notification){
        guard let keyboardSize = (notification.userInfo?[UIResponder.keyboardFrameEndUserInfoKey] as? NSValue)?.cgRectValue
        else { return }
        if notification.name == UIResponder.keyboardWillShowNotification{
            view.frame.origin.y = -keyboardSize.height
        } else if notification.name == UIResponder.keyboardWillHideNotification{
            view.frame.origin.y = 0
        }
    }
    
    deinit {
        NotificationCenter.default.removeObserver(UIResponder.keyboardWillShowNotification)
        NotificationCenter.default.removeObserver(UIResponder.keyboardWillHideNotification)
    }
}

