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
        dismiss(animated: true, completion: nil)
        print("disconnect")
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
    func keyboardChar(s: Character) -> (Int?, Int?){
        //this is dictionary translation character to keyboard code
        //map(letter,(vireual key, modification))
        let convertData  = [
            "q":[12,0], "w":[13,0], "e":[14,0], "r":[15,0], "t":[17,0],
            "y":[16,0], "u":[32,0], "i":[34,0], "o":[31,0], "p":[35,0],
            "a":[0,0],  "s":[1,0],  "d":[2,0],  "f":[3,0],  "g":[5,0],
            "h":[4,0],  "j":[38,0], "k":[40,0], "l":[37,0], "z":[6,0],
            "x":[7,0],  "c":[8,0],  "v":[9,0],  "b":[11,0], "n":[45,0],
            "m":[46,0], "1":[18,0], "2":[19,0], "3":[20,0], "4":[21,0],
            "5":[23,0], "6":[22,0], "7":[26,0], "8":[28,0], "9":[25,0],
            "0":[29,0], "-":[27,0], "/":[44,0], ";":[41,0], " ":[49,0],
            ".":[47,0], ",":[43,0], "\'":[39,0],"[":[33,0], "]":[30,0],
            "=":[24,0], "\\":[42,0],
            "Q":[12, 1],"W":[13,1], "E":[14,1], "R":[15,1], "T":[17,1],
            "Y":[16,1], "U":[32,1], "I":[34,1], "O":[31,1], "P":[35,1],
            "A":[0,1],  "S":[1,1],  "D":[2,1],  "F":[3,1],  "G":[5,1],
            "H":[4,1],  "J":[38,1], "K":[40,1], "L":[37,1], "Z":[6,1],
            "X":[7,1],  "C":[8,1],  "V":[9,1],  "B":[11,1], "N":[45,1],
            "M":[46,1], ":":[41,1], "(":[25,1], ")":[29,1], "$":[21,1],
            "&":[26,1], "@":[19,1], "\"":[39,1],"?":[44,1], "!":[18,1],
            "{":[33,1], "}":[30,1], "#":[20,1], "%":[23,1], "^":[22,1],
            "*":[28,1], "+":[24,1], "_":[27,1], "~":[50,1], "<":[43,1],
            ">":[47,1], "¥":[16,2], "£":[20,2], "•":[28,2], "€":[19,3]]
        //convert
        let res = convertData[String(s)]
        return (res?[0],res?[1])
    }
    
    deinit {
        NotificationCenter.default.removeObserver(UIResponder.keyboardWillShowNotification)
        NotificationCenter.default.removeObserver(UIResponder.keyboardWillHideNotification)
    }
}

