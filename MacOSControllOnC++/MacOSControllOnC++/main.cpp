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


void keyTab (int key, CGEventFlags flag = 0){
    //Create event
    CGEventRef keyDown = CGEventCreateKeyboardEvent(NULL, CGKeyCode(key), true);
    CGEventRef keyUp = CGEventCreateKeyboardEvent(NULL, CGKeyCode(key), false);
    //Event modification
    CGEventSetFlags(keyDown, flag);
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
    //map(letter,(vireual key, modification))
    const std::map<char,std::pair<int, CGEventFlags>> convertData =
    {
        {'q',{12,0}}, {'w',{13,0}}, {'e',{14,0}}, {'r',{15,0}}, {'t',{17,0}},
        {'y',{16,0}}, {'u',{32,0}}, {'i',{34,0}}, {'o',{31,0}}, {'p',{35,0}},
        {'a',{0,0}},  {'s',{1,0}},  {'d',{2,0}},  {'f',{3,0}},  {'g',{5,0}},
        {'h',{4,0}},  {'j',{38,0}}, {'k',{40,0}}, {'l',{37,0}}, {'z',{6,0}},
        {'x',{7,0}},  {'c',{8,0}},  {'v',{9,0}},  {'b',{11,0}}, {'n',{45,0}},
        {'m',{46,0}}, {'1',{18,0}}, {'2',{19,0}}, {'3',{20,0}}, {'4',{21,0}},
        {'5',{23,0}}, {'6',{22,0}}, {'7',{26,0}}, {'8',{28,0}}, {'9',{25,0}},
        {'0',{29,0}}, {'-',{27,0}}, {'/',{44,0}}, {';',{41,0}}, {' ',{49,0}},
        {'.',{47,0}}, {',',{43,0}}, {'\'',{39,0}},{'[',{33,0}}, {']',{30,0}},
        {'=',{24,0}}, {'\\',{42,0}},
        {'Q',{12,kCGEventFlagMaskShift}}, {'W',{13,kCGEventFlagMaskShift}},
        {'E',{14,kCGEventFlagMaskShift}}, {'R',{15,kCGEventFlagMaskShift}},
        {'T',{17,kCGEventFlagMaskShift}}, {'Y',{16,kCGEventFlagMaskShift}},
        {'U',{32,kCGEventFlagMaskShift}}, {'I',{34,kCGEventFlagMaskShift}},
        {'O',{31,kCGEventFlagMaskShift}}, {'P',{35,kCGEventFlagMaskShift}},
        {'A',{0,kCGEventFlagMaskShift}},  {'S',{1,kCGEventFlagMaskShift}},
        {'D',{2,kCGEventFlagMaskShift}},  {'F',{3,kCGEventFlagMaskShift}},
        {'G',{5,kCGEventFlagMaskShift}},  {'H',{4,kCGEventFlagMaskShift}},
        {'J',{38,kCGEventFlagMaskShift}}, {'K',{40,kCGEventFlagMaskShift}},
        {'L',{37,kCGEventFlagMaskShift}}, {'Z',{6,kCGEventFlagMaskShift}},
        {'X',{7,kCGEventFlagMaskShift}},  {'C',{8,kCGEventFlagMaskShift}},
        {'V',{9,kCGEventFlagMaskShift}},  {'B',{11,kCGEventFlagMaskShift}},
        {'N',{45,kCGEventFlagMaskShift}}, {'M',{46,kCGEventFlagMaskShift}},
        {':',{41,kCGEventFlagMaskShift}}, {'(',{25,kCGEventFlagMaskShift}},
        {')',{29,kCGEventFlagMaskShift}}, {'$',{21,kCGEventFlagMaskShift}},
        {'&',{26,kCGEventFlagMaskShift}}, {'@',{19,kCGEventFlagMaskShift}},
        {'"',{39,kCGEventFlagMaskShift}}, {'?',{44,kCGEventFlagMaskShift}},
        {'!',{18,kCGEventFlagMaskShift}}, {'{',{33,kCGEventFlagMaskShift}},
        {'}',{30,kCGEventFlagMaskShift}}, {'#',{20,kCGEventFlagMaskShift}},
        {'%',{23,kCGEventFlagMaskShift}}, {'^',{22,kCGEventFlagMaskShift}},
        {'*',{28,kCGEventFlagMaskShift}}, {'+',{24,kCGEventFlagMaskShift}},
        {'_',{27,kCGEventFlagMaskShift}}, {'~',{50,kCGEventFlagMaskShift}},
        {'<',{43,kCGEventFlagMaskShift}}, {'>',{47,kCGEventFlagMaskShift}},
        {'\¥',{16,kCGEventFlagMaskAlternate}}, {'\£',{20,kCGEventFlagMaskAlternate}},
        {'\€',{19,(kCGEventFlagMaskAlternate | kCGEventFlagMaskShift)}}
    };
    
    for(auto i : str){
        auto iter = convertData.find(i);
        keyTab(iter->second.first, iter->second.second);
    }
    keyTab(36);
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
                const std::string str = "Hello @$€";
                sleep(4);
            
                keyboardString(str);
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

