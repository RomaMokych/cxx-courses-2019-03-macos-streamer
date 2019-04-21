//
//  ViewController.swift
//  Controll_IOS_Project
//
//  Created by MacOS on 28.03.2019.
//  Copyright © 2019 MacOS. All rights reserved.
//
import CoreGraphics
import UIKit

class ViewController: UIViewController, UITextFieldDelegate {
    var send = true
    var modelData: ModelData!
    var receiver: MessageReceiver!
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
        modelData.receiver = receiver
        modelData.receiver.closeConnection(msg: "You closed connection!")

        print("Disconnected")
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        receiver = modelData.receiver
        
        //delegate
        keyboardStreamTextField.delegate = self
        keyboardStreamTextField.addTarget(self,action:#selector(textFieldChangeStream(_ :)), for: .allEditingEvents)
        
        self.view.isMultipleTouchEnabled = true
        
        //Listen keyboards events
        NotificationCenter.default.addObserver(self, selector: #selector(keyboardChange(notification:)), name: UIResponder.keyboardWillShowNotification , object: nil)
        NotificationCenter.default.addObserver(self, selector: #selector(keyboardChange(notification:)), name: UIResponder.keyboardWillHideNotification, object: nil)
        //Listen new frame
        NotificationCenter.default.addObserver(self, selector: #selector(frameChange(notification:)), name: MessageReceiver.newFrameNotification, object: nil)
        //Listen status connection
        NotificationCenter.default.addObserver(self, selector: #selector(connectionInterruption), name: MessageReceiver.connectionNotification, object: nil)
        
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
            
        } else if recognizer.state == .changed{
            //message reduction
            if send{
                send = false
            } else {
                send = true
                return
            }
            let point = recognizer.translation(in: videoView)
            
            print("Moving", point)
            
            let x : CGFloat = point.x;
            let y : CGFloat = point.y;
            
            var bytesX = toByteArray(value : x)
            var bytesY = toByteArray(value : y)
            let type = UInt8(7)
            let len = UInt(16)
            
            var size_in_bytes = toByteArray(value : len)
            
            let buffer = UnsafeMutablePointer<UInt8>.allocate(capacity: 16+4+1)
            
            for i in 0..<4 {
                buffer[i] = size_in_bytes[i]
            }
            buffer[4] = type;
            
            var j = 0;
            for i in 5..<13 {
                buffer[i] = bytesX[j];
                j+=1;
            }
            j = 0;
            for i in 13..<21 {
                buffer[i] = bytesY[j];
                j+=1;
            }
            
            receiver.outputStream.write(buffer, maxLength: 16 + 4 + 1)
            
            recognizer.setTranslation(CGPoint.zero, in: videoView)
            
        } else if recognizer.state == .ended {
            
        }
        
    }
    @objc func mouseDownMove(recognizer: UIPanGestureRecognizer){
        
        if recognizer.state == .began{
            //code 4
            print("mouse down")//transfer to server
            
            let type = UInt8(4)
            let len = UInt(1)
            
            var size_in_bytes = toByteArray(value : len)
            
            let buffer = UnsafeMutablePointer<UInt8>.allocate(capacity: 1 + 4 + 1)
            
            for i in 0..<4 {
                buffer[i] = size_in_bytes[i]
            }
            buffer[4] = type;
            buffer[5] = 1
            
            receiver.outputStream.write(buffer, maxLength: 6)
            
            
        } else if recognizer.state == .changed {
            //message reduction
            if send{
                send = false
            } else {
                send = true
                return
            }
            //code 5 and x and y
            let point = recognizer.translation(in: videoView)//transfer to server
            print("Moving and mouse down", point.x,point.y)
            
            let x : CGFloat = point.x;
            let y : CGFloat = point.y;
            
            var bytesX = toByteArray(value : x)
            var bytesY = toByteArray(value : y)
            let type = UInt8(5)
            let len = UInt(16)
            
            var size_in_bytes = toByteArray(value : len)
            
            let buffer = UnsafeMutablePointer<UInt8>.allocate(capacity: 16+4+1)
            
            for i in 0..<4 {
                buffer[i] = size_in_bytes[i]
            }
            buffer[4] = type;
            
            var j = 0;
            for i in 5..<13 {
                buffer[i] = bytesX[j];
                j+=1;
            }
            j = 0;
            for i in 13..<21 {
                buffer[i] = bytesY[j];
                j+=1;
            }
            
            receiver.outputStream.write(buffer, maxLength: 16 + 4 + 1)
            
            recognizer.setTranslation(CGPoint.zero, in: videoView)
            
        } else if recognizer.state == .ended {
            //code 6
           print("Mouse up")//transfer to server
            
            let type = UInt8(6)
            let len = UInt(1)
            
            var size_in_bytes = toByteArray(value : len)
            
            let buffer = UnsafeMutablePointer<UInt8>.allocate(capacity: 1 + 4 + 1)
            
            for i in 0..<4 {
                buffer[i] = size_in_bytes[i]
            }
            buffer[4] = type;
            buffer[5] = 1
            
            receiver.outputStream.write(buffer, maxLength: 6)
            
        }
        
    }
    @objc func leftTap(recognizer: UITapGestureRecognizer){
        if recognizer.state == .ended {
            //code 1
            print("just left tap",recognizer.numberOfTapsRequired)
            
            let type = UInt8(1)
            let len = UInt(1)
            
            var size_in_bytes = toByteArray(value : len)
            
            let buffer = UnsafeMutablePointer<UInt8>.allocate(capacity: 1 + 4 + 1)
            
            for i in 0..<4 {
                buffer[i] = size_in_bytes[i]
            }
            buffer[4] = type;
            buffer[5] = 1
            
            receiver.outputStream.write(buffer, maxLength: 6)
            
        }
    }
    @objc func leftDoubleTap(recognizer: UITapGestureRecognizer){
        if recognizer.state == .ended {
            //code 2
            print("just double left tap",recognizer.numberOfTapsRequired)
            
            let type = UInt8(2)
            let len = UInt(1)
            
            var size_in_bytes = toByteArray(value : len)
            
            let buffer = UnsafeMutablePointer<UInt8>.allocate(capacity: 1 + 4 + 1)
            
            for i in 0..<4 {
                buffer[i] = size_in_bytes[i]
            }
            buffer[4] = type;
            buffer[5] = 1
            
            receiver.outputStream.write(buffer, maxLength: 6)
            
        }
    }
    @objc func rightTap(recognize: UITapGestureRecognizer){
        if recognize.state == .ended{
            //code 3
            print("just right tap")
            
            let type = UInt8(3)
            let len = UInt(1)
            
            var size_in_bytes = toByteArray(value : len)
            
            let buffer = UnsafeMutablePointer<UInt8>.allocate(capacity: 1 + 4 + 1)
            
            for i in 0..<4 {
                buffer[i] = size_in_bytes[i]
            }
            buffer[4] = type;
            buffer[5] = 1
            
            receiver.outputStream.write(buffer, maxLength: 6)
            
        }
    }
    @objc func scroll(recognizer: UIPanGestureRecognizer){
        
        if recognizer.state == .began{
            //code 11 and y
        } else if recognizer.state == .changed {
            //message reduction
            if send{
                send = false
            } else {
                send = true
                return
            }
            
            let point = recognizer.translation(in: videoView) //transfer to server
            
            let y : CGFloat = point.y;
            
            var bytesY = toByteArray(value : y)
            let type = UInt8(11)
            let len = UInt(8)
            
            var size_in_bytes = toByteArray(value : len)
            
            let buffer = UnsafeMutablePointer<UInt8>.allocate(capacity: 8+4+1)
            
            for i in 0..<4 {
                buffer[i] = size_in_bytes[i]
            }
            buffer[4] = type;
            
            var j = 0;
            for i in 5..<13 {
                buffer[i] = bytesY[j];
                j+=1;
            }
            
            receiver.outputStream.write(buffer, maxLength: 8 + 4 + 1)
            
            
            if point.y > 0{
                print("scrollDown", point)
            } else if point.y < 0 {
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
            
            let symbol = textField.text!.popLast()!
            
            print(symbol)        //transfer to server
            
            let type = UInt8(8)
            let len = UInt(1)
            
            var size_in_bytes = toByteArray(value : len)
            
            let buffer = UnsafeMutablePointer<UInt8>.allocate(capacity: 1 + 4 + 1)
            
            for i in 0..<4 {
                buffer[i] = size_in_bytes[i]
            }
            buffer[4] = type;
            
            let s = String(symbol).utf8.map{UInt8($0)}[0]
            buffer[5] = s
            
            receiver.outputStream.write(buffer, maxLength: 6)
            
        } else
        if textField.text!.isEmpty {
            //code 10
            print("transfer to server code code 51")
            
            let type = UInt8(10)
            let len = UInt(1)
            
            var size_in_bytes = toByteArray(value : len)
            
            let buffer = UnsafeMutablePointer<UInt8>.allocate(capacity: 1 + 4 + 1)
            
            for i in 0..<4 {
                buffer[i] = size_in_bytes[i]
            }
            buffer[4] = type;
            
            let sID : UInt8 = 51;
            buffer[5] = sID
            
            receiver.outputStream.write(buffer, maxLength: 6)
            
            textField.text = "/"
        }
    }
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        //code 9
        print("transfer to server code code 36")
        
        let type = UInt8(10)
        let len = UInt(1)
        
        var size_in_bytes = toByteArray(value : len)
        
        let buffer = UnsafeMutablePointer<UInt8>.allocate(capacity: 1 + 4 + 1)
        
        for i in 0..<4 {
            buffer[i] = size_in_bytes[i]
        }
        buffer[4] = type;
        
        let sID : UInt8 = 36
        buffer[5] = sID
        
        receiver.outputStream.write(buffer, maxLength: 6)
        
        textField.text = "/"                        //clear text
        return false
    }
    
    //Mark: - Notification
    @objc func frameChange(notification: Notification){
        if let frame = (notification.userInfo?[MessageReceiver.frameInfoKey]){
            videoView.layer.contents = frame
        }
    }
    @objc func connectionInterruption(notification: Notification){
        guard let connectionStatus = (notification.userInfo?[MessageReceiver.connectionStatusInfoKey] as? Bool)
            else { return }
        if !connectionStatus {
            modelData.receiver = receiver
            dismiss(animated: true, completion: nil)
        }
    }
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
        NotificationCenter.default.removeObserver(MessageReceiver.newFrameNotification)
        NotificationCenter.default.removeObserver(MessageReceiver.connectionNotification)
    }
}

