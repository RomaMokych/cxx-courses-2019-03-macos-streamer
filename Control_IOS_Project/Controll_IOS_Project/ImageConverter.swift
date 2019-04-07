//
//  ImageConverter.swift
//  Test_iOS
//
//  Created by Nikita on 3/16/19.
//  Copyright © 2019 Nikita. All rights reserved.
//

import Foundation
import UIKit

class Converter{

private let rgbColorSpace = CGColorSpaceCreateDeviceRGB()
private let bitmapInfo:CGBitmapInfo = CGBitmapInfo(rawValue: CGImageAlphaInfo.premultipliedFirst.rawValue)

    public func imageFromARGB32Bitmap(pixels:UnsafeMutablePointer<UInt8>, width:Int, height:Int)->CGImage? {
        
        guard width > 0 && height > 0 else { return nil }
        
        let rgbColorSpace = CGColorSpaceCreateDeviceRGB()
        let bitmapInfo = CGBitmapInfo(rawValue: CGImageAlphaInfo.premultipliedFirst.rawValue)
        let bitsPerComponent = 8
        let bitsPerPixel = 32
        
        let data = pixels // Copy to mutable []
        guard let providerRef = CGDataProvider(data: NSData(bytes: data,
                                                            length: Int(width*height * 4))
            )
            else { return nil }
        
        guard let cgim = CGImage(
            width: Int(width),
            height: Int(height),
            bitsPerComponent: bitsPerComponent,
            bitsPerPixel: bitsPerPixel,
            bytesPerRow: Int(width) * 4,
            space: rgbColorSpace,
            bitmapInfo: bitmapInfo,
            provider: providerRef,
            decode: nil,
            shouldInterpolate: true,
            intent: .defaultIntent
            )
            else { return nil }
        
        return cgim
    }
}
