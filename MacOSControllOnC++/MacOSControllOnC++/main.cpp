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


void keyTab (const unsigned short &key,const CGEventFlags &flag = 0){
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
CGEventFlags convertModFlag(const int &flag){
    switch (flag) {
        case 1:
            return kCGEventFlagMaskShift;
        case 2:
            return kCGEventFlagMaskAlternate;
        case 3:
            return kCGEventFlagMaskShift | kCGEventFlagMaskAlternate;
        default:
            return 0;
    }
}
void mouseTab (const CGMouseButton &mouseButton, const bool &doubleClick = false){
    //Create type event
    CGEventType mouseTypeUp;
    CGEventType mouseTypeDown;
    
    switch (mouseButton){
        case kCGMouseButtonLeft:{
            mouseTypeUp = kCGEventLeftMouseUp;
            mouseTypeDown = kCGEventLeftMouseDown;
            break;}
        case kCGMouseButtonRight:{
            mouseTypeUp = kCGEventRightMouseUp;
            mouseTypeDown = kCGEventRightMouseDown;
            break;}
        case kCGMouseButtonCenter:{
            mouseTypeUp = kCGEventOtherMouseUp;
            mouseTypeDown = kCGEventOtherMouseDown;
        }
        default:
            return;
    }
    //Get mouse location
    CGEventRef eventLocation = CGEventCreate(NULL);
    CGPoint point = CGEventGetLocation(eventLocation);
    //Free event
    CFRelease(eventLocation);
    //Create mouse event
    auto mouseDown = CGEventCreateMouseEvent(NULL, mouseTypeDown, point, mouseButton);
    auto mouseUp = CGEventCreateMouseEvent(NULL, mouseTypeUp, point, mouseButton);
    //Post event
    CGEventPost(kCGHIDEventTap, mouseDown);
    CGEventPost(kCGHIDEventTap, mouseUp);
    if(doubleClick){
        CGEventSetIntegerValueField(mouseDown, kCGMouseEventClickState, 2);
        CGEventSetIntegerValueField(mouseUp, kCGMouseEventClickState, 2);
        CGEventPost(kCGHIDEventTap, mouseDown);
        CGEventPost(kCGHIDEventTap, mouseUp);
    }
    //Free event
    CFRelease(mouseUp);
    CFRelease(mouseDown);
}
void leftMouseDownUP (const CGEventType &mouseEvent){
    //Get mouse location
    CGEventRef event = CGEventCreate(NULL);
    CGPoint point = CGEventGetLocation(event);
    //Free event
    CFRelease(event);
    //Create mouse event
    auto mouseDownUp = CGEventCreateMouseEvent(NULL, mouseEvent, point, kCGMouseButtonLeft);
    //Create mouse event
    CGEventPost(kCGHIDEventTap, mouseDownUp);
    //Free event
    CFRelease(mouseDownUp);
}
void mouseMove(CGEventType event,const double &x,const double &y){
    //Get mouse location
    CGEventRef eventLocation = CGEventCreate(NULL);
    CGPoint mouseLocation = CGEventGetLocation(eventLocation);
    mouseLocation.x += x;
    mouseLocation.y += y;
    //Free event
    CFRelease(eventLocation);
    //Mouse move
    CGEventRef move = CGEventCreateMouseEvent(NULL, event, mouseLocation, kCGMouseButtonLeft);
    CGEventPost(kCGHIDEventTap, move);
    //Free event
    CFRelease(move);
}
void keyboardChar(const char &s){
    //this is dictionary translation character to keyboard code
    //map(letter,(vireual key, modification))
    const std::map<char,std::pair<unsigned short, CGEventFlags>> convertData =
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
        {'\•',{28,kCGEventFlagMaskAlternate}},
        {'\€',{19,(kCGEventFlagMaskAlternate | kCGEventFlagMaskShift)}}
    };
    //find virtual code and tab
    auto iterPair = convertData.find(s);
    if(iterPair != convertData.end())
        keyTab(iterPair->second.first,iterPair->second.second);
}
void scrollMove(const int wheel){
    //Create event
    CGEventRef scrollEvent;
    if(wheel < -5 || 5 < wheel){
        //Apple recomends values up to |10|
        short pixel;
        size_t quantity;
        if(wheel > 0){
            pixel = 5;
        } else {
            pixel = -5;
        }
        quantity = wheel/pixel;
        scrollEvent = CGEventCreateScrollWheelEvent(NULL, kCGScrollEventUnitPixel, 1/*magic number*/, pixel);
        for(size_t i(0); i < quantity; ++i)
            //Release event
            CGEventPost(kCGHIDEventTap, scrollEvent);
    } else {
        scrollEvent = CGEventCreateScrollWheelEvent(NULL, kCGScrollEventUnitPixel, 1/*magic number*/, wheel);
        //Release event
        CGEventPost(kCGHIDEventTap, scrollEvent);
    }
    //Free event
    CFRelease(scrollEvent);
}

int main(int argc, const char * argv[]) {
    int command = 0;
    int x = 0,y = 0;
    char c;
    switch (command){
        case 1:{
            mouseTab(kCGMouseButtonLeft);
            break;}
        case 2:{
            mouseTab(kCGMouseButtonLeft, true);
            break;}
        case 3:{
            mouseTab(kCGMouseButtonRight);
            break;}
        case 4:{
            leftMouseDownUP(kCGEventLeftMouseDown);
            break;}
        case 5:{
            mouseMove(kCGEventLeftMouseDragged, x, y);
            break;}
        case 6:{
            leftMouseDownUP(kCGEventLeftMouseUp);
            break;}
        case 7:{
            mouseMove(kCGEventMouseMoved, x, y);
            break;}
        case 8:{
            keyboardChar(c);
            break;}
        case 9:{
            keyTab(36);
            break;}
        case 10:{
            keyTab(51);
            break;}
        case 11:{
            scrollMove(y);
            break;}
        default:{
            
        }
    }
    
    
    /*bool work(true);
    std::cout << "Hello\n";
    while(work){
        std::cout << "Enter 1 for test mouse\n" <<
        "Enter 2 for test keyboard (need open text field)\n" <<
        "Enter 3 for test exit\n" <<
        "Enter 4 for test scroll\n" <<
        "Enter 5 for test mouse move dragging\n"<<
        "Enter 6 for test double click\n";
        short testId;
        std::cin >> testId;
        if(testId > 6 || testId < 1){
            std::cout << "error input";
            continue;
        }
        switch (testId) {
            case 1:{
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
                break;}
            case 2:{
                const std::string str = "Hello @$€";
                sleep(4);
                for(size_t i(0); i < str.size(); ++i)
                    keyboardChar(str[i]);
                break;}
            case 3:{
                work = false;
                break;}
            case 4:{
                mouseTab(kCGMouseButtonLeft);
                scrollMove(100);
                break;}
            case 5:{
                leftMouseDownUP(kCGEventLeftMouseDown);
                for(size_t i(100); i > 0; --i){
                    usleep(10000);
                    mouseMove(kCGEventLeftMouseDragged, 2, 2);
                }
                leftMouseDownUP(kCGEventLeftMouseUp);
                break;}
            case 6:{
                mouseTab(kCGMouseButtonLeft,true);
                break;}
    
            default:{
                std::cout << "Error input";}
        }
    
    }*/
}

