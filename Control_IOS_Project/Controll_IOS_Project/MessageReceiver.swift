//
//  File.swift
//  Test_iOS
//
//  Created by Nikita on 3/15/19.
//  Copyright © 2019 Nikita. All rights reserved.
//

import Foundation
import UIKit


@objcMembers class MessageReceiver : NSObject
{
    static var ip = "localhost"
    
    var controller : ViewController?

    var inputStream: InputStream!
    var outputStream: OutputStream!
    
    let maxReadLength = 4096
    
    // ---------------SCREEN SIZE--------------
    var BaseWidth = 1920, BaseHeight = 1080;
    // Please change width to 1280 and height to 720
    //------------------------------------------
    
    var BaseBytesCount : Int?;
    var read = 0;
    
    let converter = Converter();
    var buffer: UnsafeMutablePointer<UInt8>?
    
    
    func setupNetworkCommunication() {
        
        var readStream: Unmanaged<CFReadStream>?
        var writeStream: Unmanaged<CFWriteStream>?
        
        // -------INSERT YOUR IP HERE----------
        
        let ipAddress = MessageReceiver.ip;
        print(ipAddress)
        // ------------------------------------
        
        CFStreamCreatePairWithSocketToHost(kCFAllocatorDefault,
                                           ipAddress as CFString,
                                           9999,
                                           &readStream,
                                           &writeStream);
        
        inputStream = readStream!.takeRetainedValue()
        outputStream = writeStream!.takeRetainedValue()
        
        inputStream.delegate = self
        
        BaseBytesCount = self.BaseWidth * self.BaseHeight * 4;
        print("base byte count = ", BaseBytesCount)
        
        buffer = UnsafeMutablePointer<UInt8>.allocate(capacity: BaseBytesCount!)
        
        inputStream.schedule(in: .current, forMode: .default)
        outputStream.schedule(in: .current, forMode: .default)
        
        inputStream.open()
        outputStream.open()
    }
    
    
    func stopSession() {
        
        inputStream.close()
        outputStream.close()
        
        UIGraphicsEndImageContext();
        free(buffer)
    }
    
}
extension MessageReceiver: StreamDelegate {
    
    func stream(_ aStream: Stream, handle eventCode: Stream.Event) {
    switch eventCode {
        case Stream.Event.hasBytesAvailable:
            
            // ------CAN SLOW DOWN SYSTEM------
             print("new message received")
             // -------------------------------
             
            readAvailableBytes(stream: aStream as! InputStream)

        case Stream.Event.endEncountered:
            print("new message received(end?)")
            stopSession()
        case Stream.Event.errorOccurred:
            print("error occurred")
        case Stream.Event.hasSpaceAvailable:
            print("has space available")
        default:
            print("some other event...")
        break
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
                
                controller?.videoView.layer.contents = converter.imageFromARGB32Bitmap(pixels: buffer!, width: BaseWidth, height: BaseHeight);
                read = 0;
            }else if ((numberOfBytesRead + read) < BaseBytesCount!){
                read += numberOfBytesRead;
            }else if( (numberOfBytesRead + read) > BaseBytesCount!){
                read = 0; // drop it
            }
            
          
        }
    }

    
}

