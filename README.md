# cxx-courses-2019-03-macos-streamer
Project name:
Mac OS Streamer

Description:
Application for iOS-based devices that allows you to stream your Mac OS X's desktop image and control your Mac OS X PC/ laptop in realtime using touches, gestures and keyboard.
Server-side is also included.

Usage:

For users:
1) Install SRD_Server on your Mac OS X PC/laptop and run it.
2) Install SRD_Cient on your iOS device, enter running server's IP in the text field, and press "Connect". 
   (You will be notified if something wrong happens, or you see Mac OS X desktop)
3) Enjoy using remote computer like this :
   one tap -> single click;
   two taps -> double click
   two-finger tap -> right click;
   two-fingers down and movement -> pressing and moving 'left mouse button' on server;
   three-fingers up/down -> scrolling
4) Stop communication from iOS by pressing Power-off icon on the top right corner of the screen;
5) Stop communication from Mac OS X by entering '/stop' in the console window.



For devs:

First of all, find out Mac OS X's running server IP or run your own server by opening and compiling 'SRD_Server.xcodeproj' from 'SRD_Server/' folder.
Then, compile and install iOS appication on your iPhone/iPad (SRD_Client/SRD_Client.xcodeproj), launch it, enter Mac OS X server IP and tap "Connect".
