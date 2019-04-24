#include "InputManager.hpp"
void InputManager::moveMouseTo(const double& x, const double& y)
{
    cout << "InputManager::MoveMouseTo(" << x << "," << y << ")\n";
    mouseMove(kCGEventMouseMoved, x, y);
}


void InputManager::keyTab (const unsigned short &key,const CGEventFlags &flag){
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
CGEventFlags InputManager::convertModFlag (const int &flag){
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

void InputManager::mouseTab (const CGMouseButton& mouseButton, const bool& doubleClick){
    
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

void InputManager::leftMouseDownUP (const CGEventType &mouseEvent){
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

void InputManager::mouseMove(CGEventType event,const double &x,const double &y){
    //Get mouse location
    CGEventRef eventLocation = CGEventCreate(NULL);
    
    CGPoint mouseLocation = CGEventGetLocation(eventLocation);
    
    mouseLocation.x += x;
    mouseLocation.y += y;
    //cout << "Modified" << mouseLocation.x << " ; " << mouseLocation.y << "\n";
    
    // Hide this duplication. We've used this earlier to handle mouse movement because of incorrect behaviour of events
    // CGWarpMouseCursorPosition(CGPointMake(mouseLocation.x, mouseLocation.y));
    
    //Free event
    CFRelease(eventLocation);
    //Mouse move
    CGEventRef move = CGEventCreateMouseEvent(NULL, event, mouseLocation, kCGMouseButtonLeft);
    CGEventPost(kCGHIDEventTap, move);
    //Free event
    CFRelease(move);
}


void InputManager::scrollMove(const int& wheel){
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

void InputManager::press_LeftMouseButton(bool doubleClick) {
    cout << "Press left\n";
    mouseTab(kCGMouseButtonLeft, doubleClick);
}

void InputManager::press_RightMouseButton() {
    cout << "Press right\n";
    mouseTab(kCGMouseButtonRight);
}

void InputManager::hold_LeftMouseButton() {
    cout << "Hold left\n";
    leftMouseDownUP(kCGEventLeftMouseDown);
}

void InputManager::move_MouseDraggedTo(const double &x, const double &y) {
    cout << " move_MouseDraggedTo\n";
    mouseMove(kCGEventLeftMouseDragged, x, y);
}

void InputManager::free_LeftMouseButton() {
    cout << " free_LeftMouseButton\n";
    leftMouseDownUP(kCGEventLeftMouseUp);
}

void InputManager::move_MouseTo(const double &x, const double &y) {
    cout << " move_MouseTo\n";
    mouseMove(kCGEventMouseMoved, x, y);
}

void InputManager::press_KeyTab(const int& virtualKey, const int& keyMod) {
    cout << " press_KeyTab\n";
    keyTab(virtualKey, convertModFlag(keyMod));
}

void InputManager::scroll(const int &y) {
    cout << " scroll\n";
    scrollMove(y);
}
