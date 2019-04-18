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
    
    func fromByteArray<T>(value: [UInt8], _: T.Type) -> T
    {
        return value.withUnsafeBytes { $0.baseAddress!.load(as: T.self) }
    }
    
    func setupNetworkCommunication() {
        
        var readStream: Unmanaged<CFReadStream>?
        var writeStream: Unmanaged<CFWriteStream>?
        
        // -------INSERT YOUR IP HERE----------
        
        let ipAddress = "localhost";
        print(ipAddress)
        // ------------------------------------
        
        
        
        
        CFStreamCreatePairWithSocketToHost(kCFAllocatorDefault,
                                           ipAddress as CFString,
                                           2220,
                                           &readStream,
                                           &writeStream);
        
        inputStream = readStream!.takeRetainedValue()
        outputStream = writeStream!.takeRetainedValue()

        //var socketStream = CFSocketCreate(kCFAllocatorDefault, AF_INET, SOCK_STREAM, 0, 2, nil, nil)
        
        
        
       // let socketData = CFWriteStreamCopyProperty(self.outputStream!, CFStreamPropertyKey.socketNativeHandle) as! CFData
        //let handle = CFSocketNativeHandle(CFDataGetBytePtr(socketData)?.pointee)
        
       
        
       
        
        inputStream.delegate = self
        
        BaseBytesCount = self.BaseWidth * self.BaseHeight * 4;
        print("base byte count = ", BaseBytesCount!)
        
        buffer = UnsafeMutablePointer<UInt8>.allocate(capacity: BaseBytesCount!)
        
        inputStream.schedule(in: .current, forMode: .default)
        outputStream.schedule(in: .current, forMode: .default)
        
        inputStream.open()
        outputStream.open()
//
//        var socketData = CFWriteStreamCopyProperty(self.outputStream!, CFStreamPropertyKey.socketNativeHandle) as! NSData
//        var socket: CFSocketNativeHandle = 0
//
//        let x = MemoryLayout<CFSocketNativeHandle>.size
//        var one: UInt32 = 1;
//
//        socketData.getBytes(&socket, length: x)
//
//        setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, &one, socklen_t(x))
        
//        var socketData = CFWriteStreamCopyProperty(outputStream, CFStreamPropertyKey.socketNativeHandle) as! NSData
//        var socket: CFSocketNativeHandle = 0
//        let x = MemoryLayout<CFSocketNativeHandle>.size
//
//        socketData.getBytes(&socket, length: x)
//
//        var on: UInt32 = 1;
//        if setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, &on, socklen_t(x)) == -1 {
//            print("Oh...")
//        }
        
    }
    
    func connect(Ip4: String) -> Bool{
        //lodic func
        return true
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

