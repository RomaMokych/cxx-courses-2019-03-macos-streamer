import Foundation
import Cocoa
//_ = NSEvent.mouseLocation

func keyTab (key: Int){
    let keyDown = CGEvent(keyboardEventSource: nil, virtualKey: CGKeyCode(key), keyDown: true)
    let keyUp = CGEvent(keyboardEventSource: nil, virtualKey: CGKeyCode(key), keyDown: false)
    keyDown?.post(tap: .cghidEventTap)
    keyUp?.post(tap: .cghidEventTap)
}
func mouseTab (mouseEvent: CGMouseButton){
    var mouseLocation = NSEvent.mouseLocation
    let mouseTypeUp: CGEventType!
    let mouseTypeDown: CGEventType!
    
    switch mouseEvent{
        case .left:
            mouseTypeUp = .leftMouseUp
            mouseTypeDown = .leftMouseDown
        case .right:
            mouseTypeUp = .leftMouseUp
            mouseTypeDown = .leftMouseDown
        case .center:
            mouseTypeUp = .otherMouseUp
            mouseTypeDown = .otherMouseDown
    }
    //translate to another coordinate system
    mouseLocation.y = NSHeight(NSScreen.screens[0].frame) - mouseLocation.y
    
    let point = CGPoint(x: mouseLocation.x, y: mouseLocation.y)
    let mouseDown = CGEvent(mouseEventSource: nil, mouseType: mouseTypeDown, mouseCursorPosition: point, mouseButton: CGMouseButton(rawValue: mouseEvent.rawValue)!)
    let mouseUp = CGEvent(mouseEventSource: nil, mouseType: mouseTypeUp, mouseCursorPosition: point, mouseButton: CGMouseButton(rawValue: mouseEvent.rawValue)!)
    mouseDown?.post(tap: .cghidEventTap)
    mouseUp?.post(tap: .cghidEventTap)
}

var work = true
print("Hello!")
while work {
    print("Enter 1 for test mouse")
    print("Enter 2 for test keyboard (need open text field)")
    print("Enter 3 for test exit")
    
    let testId = Int(readLine()!)
    if testId == nil
    {
        print("error input")
        continue
    }
    switch testId {
    case 1:
        //Test 1: mouse move
        
        print("Input center of the circle posiseon x:")
        let x = Double(readLine()!)
        print("Input center of the circle posiseon y:")
        let y = Double(readLine()!)
        print("Input radius of the circle:")
        let r = Double(readLine()!)
        if x == nil || y == nil || r == nil
        {
            print("error input")
            continue
        }
        for index in 0...360{
            let radian = 57.0
            let point = CGPoint(x: x! + (r! * cos(Double(index)/radian)), y: y! + (r! * sin(Double(index)/radian)))
            CGWarpMouseCursorPosition(point)
            usleep(10000)
        }
        
        print("Ooops I open apple.")
        CGWarpMouseCursorPosition(CGPoint(x: 40, y: 20))
        mouseTab(mouseEvent: .left)
        break
    case 2:
        //Test 2: keyboard (need open text field)
        let google = [5,31,31,5,37,14]
        sleep(4)
        
        for i in google{
            usleep(10000)
            keyTab(key: i)
        }
        break
    case 3:
        work = false
        break
    default:
        print("Error enter")
        break
    }
}
    







