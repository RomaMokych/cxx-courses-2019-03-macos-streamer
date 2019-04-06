//
//  main.cpp
//  MacOSControllOnC++
//
//  Created by MacOS on 30.03.2019.
//  Copyright © 2019 MacOS. All rights reserved.
//

#include <iostream>
#include <CoreGraphics/CoreGraphics.h>


void keyTab (int key){
    CGEventRef keyDown = CGEventCreateKeyboardEvent(NULL, CGKeyCode(key), true);
    CGEventRef keyUp = CGEventCreateKeyboardEvent(NULL, CGKeyCode(key), false);
    CGEventPost(kCGHIDEventTap, keyDown);
    CGEventPost(kCGHIDEventTap, keyUp);
}
void mouseTab (CGMouseButton mouseEvent){
    CGEventType mouseTypeUp;
    CGEventType mouseTypeDown;
    
    switch (mouseEvent){
        case kCGMouseButtonLeft:
            {
                mouseTypeUp = kCGEventLeftMouseUp;
                mouseTypeDown = kCGEventLeftMouseDown;
                break;
            }
        case kCGMouseButtonRight:
        {
            mouseTypeUp = kCGEventRightMouseUp;
            mouseTypeDown = kCGEventRightMouseDown;
            std::cout << "Right\n";
            break;
        }
        default:
            return;
    }
    //get mouse location
    CGEventRef event = CGEventCreate(NULL);
    CGPoint point = CGEventGetLocation(event);
    //CFRelease(event);
    
    auto mouseDown = CGEventCreateMouseEvent(NULL, mouseTypeDown, point, mouseEvent);
    auto mouseUp = CGEventCreateMouseEvent(NULL, mouseTypeUp, point, mouseEvent);
    CGEventPost(kCGHIDEventTap, mouseDown);
    CGEventPost(kCGHIDEventTap, mouseUp);
}

int main(int argc, const char * argv[]) {
    bool work(true);
    std::cout << "Hello\n";
    while(work){
        std::cout << "Enter 1 for test mouse\n" <<
        "Enter 2 for test keyboard (need open text field)\n" <<
        "Enter 3 for test exit\n";
        unsigned short testId;
        std::cin >> testId;
        if(testId > 3){
            std::cout << "error input";
            continue;
        }
        switch (testId) {
            case 1:
                std::cout << "Input center of the circle posiseon x:\n";
                unsigned short x;
                std::cin >> x;
                std::cout << "Input center of the circle posiseon y:\n";
                unsigned short y;
                std::cin >> y;
                std::cout << "Input radius of the circle:\n";
                unsigned short r;
                std::cin >> r;
                for(int i(0);i < 360; ++i){
                    const double radian = 57.0;
                    auto point = CGPointMake(x + (r * cos(i/radian)), y + (r * sin(i/radian)));
                    CGWarpMouseCursorPosition(point);
                    usleep(10000);
                }
                break;
            case 2:{
                const int google[6] = {5,31,31,5,37,14};
                sleep(4);
            
                for(int i : google){
                    keyTab(i);
                }
                break;
            }
            case 3:{
                work = false;
                break;
            }
                
            default:
                std::cout << "Error input";
                break;
        }
    
    }
}

