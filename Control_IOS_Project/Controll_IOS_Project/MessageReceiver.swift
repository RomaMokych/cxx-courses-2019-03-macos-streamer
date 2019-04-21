//
//  File.swift
//  Test_iOS
//
//  Created by Nikita on 3/15/19.
//  Copyright © 2019 Nikita. All rights reserved.
//

import Foundation
import UIKit
import CoreFoundation
import CFNetwork

@objcMembers class MessageReceiver : NSObject
{
    static let connectionNotification = Notification.Name("connection")
    static let newFrameNotification = Notification.Name("newFrame")
    
    var controller : MainViewController?
    var imageViewController : ViewController?
    
    let converter = Converter();
    var connectionTimer : Timer?
    let timeout = 5.0
    
    var buffer: UnsafeMutablePointer<UInt8>?
    
    var inputStream: InputStream!
    var outputStream: OutputStream!
    
    let maxReadLength = 4096
    var connected  = false
    
    var BaseWidth = 1920, BaseHeight = 1080;   // Expected screen size

    var BaseBytesCount : Int?;
    var read = 0;
    
    init(mainViewController : MainViewController)
    {
        controller = mainViewController
    }
    
    func fromByteArray<T>(value: [UInt8], _: T.Type) -> T
    {
        return value.withUnsafeBytes { $0.baseAddress!.load(as: T.self) }
    }
    
    // Just sets up everything to create connection later and manage it in the delegate function
    func setupNetworkCommunication(ipAddress: String) {
        
        var readStream: Unmanaged<CFReadStream>?
        var writeStream: Unmanaged<CFWriteStream>?
        
        print("Connecting to ", ipAddress)
        
        CFStreamCreatePairWithSocketToHost(kCFAllocatorDefault,
                                           ipAddress as CFString,
                                           9999,
                                           &readStream,
                                           &writeStream);
        
        inputStream = readStream!.takeRetainedValue()
        outputStream = writeStream!.takeRetainedValue()

        inputStream.delegate = self
        
        BaseBytesCount = self.BaseWidth * self.BaseHeight * 4;
        
        buffer = UnsafeMutablePointer<UInt8>.allocate(capacity: BaseBytesCount!)
        
        inputStream.schedule(in: .current, forMode: .default)
        outputStream.schedule(in: .current, forMode: .default)
        
        inputStream.open()
        outputStream.open()
        
        launchConnectionTimeoutTimer()
        
    }
    
    func launchConnectionTimeoutTimer()
    {
        self.connectionTimer = Timer.scheduledTimer(withTimeInterval: self.timeout, repeats: false) { _ in
            if !self.connected {
                self.closeConnection(msg: "Connection error occured!")
            }
            self.connectionTimer?.invalidate()
            self.connectionTimer = nil
        }
    }
    
    func stopSession() {
        
        inputStream.close()
        outputStream.close()
        
        UIGraphicsEndImageContext();
        
        if buffer != nil{
            free(buffer)
            buffer = nil
        }
    }
    
}


extension MessageReceiver: StreamDelegate {
    
    func closeConnection(msg: String)
    {
        stopSession()
        connected = false
        //imageViewController?.performSegue(withIdentifier: "goToMainViewController", sender: self)
        //controller?.displayConnectError(msg: msg)
        NotificationCenter.default.post(name: MessageReceiver.connectionNotification, object: nil, userInfo: ["connect": false, "msg": msg])
    }
    
    func stream(_ aStream: Stream, handle eventCode: Stream.Event)
    {
    switch eventCode {
        
    case Stream.Event.openCompleted:
        if connected == false
        {
            connected = true;
            print("Connected!")
            print("Expected image size in bytes : ", BaseBytesCount!)
            
            //controller?.switchToRenderView();
            NotificationCenter.default.post(name: MessageReceiver.connectionNotification, object: nil, userInfo: ["connect": true])
        }
        
        case Stream.Event.errorOccurred:
            print("Streaming error occured")
            closeConnection(msg: "Streaming error occured")
        
        case Stream.Event.endEncountered:
            print("Other side has been disconnected")
            closeConnection(msg: "Other side has been disconnected")
        
        case Stream.Event.hasBytesAvailable:
            
            // ------CAN SLOW DOWN SYSTEM------
             print("new message received")
             // -------------------------------
             
            readAvailableBytes(stream: aStream as! InputStream)

        case Stream.Event.hasSpaceAvailable:
            print("Has space available")
        
        default:
            print("Some other event received")
    }
    }
    

    private func readAvailableBytes(stream: InputStream) {
        
      
        while stream.hasBytesAvailable {
           
            let numberOfBytesRead = inputStream.read(buffer!+read, maxLength: maxReadLength)
        
            if numberOfBytesRead < 0 {
                if let _ = stream.streamError {
                    break
                }
            }else if ((numberOfBytesRead + read) == BaseBytesCount!){ // Received frame
               // print((numberOfBytesRead + read))
               // print("READ : " , numberOfBytesRead);
                
                imageViewController?.videoView.layer.contents = converter.imageFromARGB32Bitmap(pixels: buffer!, width: BaseWidth, height: BaseHeight);
                //let frame = converter.imageFromARGB32Bitmap(pixels: buffer!, width: BaseWidth, height: BaseHeight)!
                //NotificationCenter.default.post(name: MessageReceiver.newFrameNotification, object: nil, userInfo: ["frame": frame])
                read = 0;
                
            }else if ((numberOfBytesRead + read) < BaseBytesCount!){
                read += numberOfBytesRead;
            }else if( (numberOfBytesRead + read) > BaseBytesCount!){
                read = 0; // drop it
            }
            
          
        }
    }

    
}

