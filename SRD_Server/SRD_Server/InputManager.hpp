#ifndef InputManager_hpp
#define InputManager_hpp

#include "GraphicsHeaders.h"

#include <string>
#include <map>

#include "RegularHeaders.h"


class InputManager
{
public :
    void press_LeftMouseButton(bool);
    void press_RightMouseButton();
    
    void hold_LeftMouseButton();
    void move_MouseDraggedTo(const double& x, const double& y);
    void free_LeftMouseButton();
    void move_MouseTo(const double& x, const double& y);
    
    void press_KeyboardChar(const char& c);
    void press_KeyTab(const char& c);
    
    void scroll(const int& y);
    
private:
    void mouseTab (const CGMouseButton& mouseButton, const bool& doubleClick = false);
    void keyboardChar(const char &s);
    void scrollMove(const int& wheel);
    void leftMouseDownUP(const CGEventType &mouseEvent);
    void mouseMove(CGEventType event,const double &x,const double &y);
    void keyTab (const unsigned short &key,const CGEventFlags &flag = 0);
    void moveMouseTo(const double& x, const double& y);
};

#endif /* InputManager_hpp */
