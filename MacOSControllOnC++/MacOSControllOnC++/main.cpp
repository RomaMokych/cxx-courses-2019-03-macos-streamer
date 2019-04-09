//
//  main.cpp
//  MacOSControllOnC++
//
//  Created by MacOS on 30.03.2019.
//  Copyright © 2019 MacOS. All rights reserved.
//

#include <iostream>
#include <string>
#include <map>
#include <CoreGraphics/CoreGraphics.h>
#include <CoreFoundation/CoreFoundation.h>


void keyTab (int key, bool shift = false){
    //Create event
    CGEventRef keyDown = CGEventCreateKeyboardEvent(NULL, CGKeyCode(key), true);
    CGEventRef keyUp = CGEventCreateKeyboardEvent(NULL, CGKeyCode(key), false);
    //Event modification
    if(shift)
        CGEventSetFlags(keyDown, kCGEventFlagMaskShift);
    //Release event
    CGEventPost(kCGHIDEventTap, keyDown);
    CGEventPost(kCGHIDEventTap, keyUp);
    //Free event
    CFRelease(keyDown);
    CFRelease(keyUp);
}
void mouseTab (CGMouseButton mouseEvent){
    //Create type event
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
    //Free event
    CFRelease(event);
    //Create mouse event
    auto mouseDown = CGEventCreateMouseEvent(NULL, mouseTypeDown, point, mouseEvent);
    auto mouseUp = CGEventCreateMouseEvent(NULL, mouseTypeUp, point, mouseEvent);
    CGEventPost(kCGHIDEventTap, mouseDown);
    CGEventPost(kCGHIDEventTap, mouseUp);
    //Free event
    CFRelease(mouseUp);
    CFRelease(mouseDown);
}
void keyboardString(std::string str){
    //map(letter,(vireual key, shift modification))
    const std::map<char,std::pair<int,bool>> convertData =
    {
        {'q',{12,false}}, {'w',{13,false}}, {'e',{14,false}}, {'r',{15,false}}, {'t',{17,false}},
        {'y',{16,false}}, {'u',{32,false}}, {'i',{34,false}}, {'o',{31,false}}, {'p',{35,false}},
        {'a',{0,false}}, {'s',{1,false}}, {'d',{2,false}}, {'f',{3,false}}, {'g',{5,false}},
        {'h',{4,false}}, {'j',{38,false}}, {'k',{40,false}}, {'l',{37,false}},{'z',{6,false}},
        {'x',{7,false}}, {'c',{8,false}}, {'v',{9,false}}, {'b',{11,false}}, {'n',{45,false}},{'m',{46,false}},
        {'Q',{12,true}}, {'W',{13,true}}, {'E',{14,true}}, {'R',{15,true}}, {'T',{17,true}},
        {'Y',{16,true}}, {'U',{32,true}}, {'I',{34,true}}, {'O',{31,true}}, {'P',{35,true}},
        {'A',{0,true}}, {'S',{1,true}}, {'D',{2,true}}, {'F',{3,true}}, {'G',{5,true}},
        {'H',{4,true}}, {'J',{38,true}}, {'K',{40,true}}, {'L',{37,true}},{'Z',{6,true}},
        {'X',{7,true}}, {'C',{8,true}}, {'V',{9,true}}, {'B',{11,true}}, {'N',{45,true}},{'M',{46,true}},
         {'1',{18,false}}, {'2',{19,false}}, {'3',{20,false}}, {'4',{21,false}}, {'5',{23,false}},
        {'6',{22,false}}, {'7',{26,false}}, {'8',{28,false}}, {'9',{25,false}}, {'0',{29,false}},
        {'-',{27,false}}, {'/',{44,false}}, {':',{41,true}}, {';',{41,false}}, {'(',{25,true}},
        {')',{29,true}}, {'$',{21,true}}, {' ',{49,false}}, {'&',{26,true}}, {'@',{19,true}},
        {'"',{39,true}}, {'.',{47,false}}, {',',{43,false}}, {'?',{44,true}}, {'!',{18,true}},
        {'\'',{39,false}}, {'[',{33,false}}, {']',{30,false}}, {'{',{33,true}}, {'}',{30,true}},
        {'#',{20,true}}, {'%',{23,true}}, {'^',{22,true}}, {'*',{28,true}}, {'+',{24,true}},
        {'=',{24,false}}, {'_',{27,true}}, {'\\',{42,false}}, {'~',{50,true}}, {'<',{43,true}},
        {'>',{47,true}},
    };
    
    for(auto i : str){
        auto iter = convertData.find(i);
        keyTab(iter->second.first, iter->second.second);
    }
}
void scrollMove(int wheel){
    //Create event
    CGEventRef scrollEvent;
    if(wheel < -5 || 5 < wheel){
        int quantity = abs(wheel / 5);
        scrollEvent = CGEventCreateScrollWheelEvent(NULL, kCGScrollEventUnitPixel, 1, 5);
        for(int i(0); i < quantity; ++i)
            //Release event
            CGEventPost(kCGHIDEventTap, scrollEvent);
    } else {
        scrollEvent = CGEventCreateScrollWheelEvent(NULL, kCGScrollEventUnitPixel, 1, wheel);
        //Release event
        CGEventPost(kCGHIDEventTap, scrollEvent);
    }
    //Free event
    CFRelease(scrollEvent);
}

int main(int argc, const char * argv[]) {
    bool work(true);
    std::cout << "Hello\n";
    while(work){
        std::cout << "Enter 1 for test mouse\n" <<
        "Enter 2 for test keyboard (need open text field)\n" <<
        "Enter 3 for test exit\n" <<
        "Enter 4 for test scroll\n";
        short testId;
        std::cin >> testId;
        if(testId > 4 || testId < 1){
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
            case 4:{
                mouseTab(kCGMouseButtonLeft);
                scrollMove(100);
            }
                
            default:
                std::cout << "Error input";
                break;
        }
    
    }
}

