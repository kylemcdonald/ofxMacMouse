//
//  ofxMacMouseStealer.cpp
//
//  Created by ISHII 2bit on 2015/07/21.
//
//

#include "ofxMacMouseEventStealer.h"
#include "ofLog.h"

#include <ApplicationServices/ApplicationServices.h>

namespace ofxMacMouseEventStealer {
    ofEvent<ofxMacMouseEventArg> ofxMacMouseEvent;
    
    namespace {
        CGEventRef mouseStealer(CGEventTapProxy proxy,
                                CGEventType type,
                                CGEventRef event,
                                void *info)
        {
            ofxMacMouseEventType mouseEvent;
            switch (type) {
                case kCGEventLeftMouseDown:
                    mouseEvent = OFX_MAC_MOUSE_EVENT_LEFT_BUTTON_DOWN;
                    break;
                case kCGEventLeftMouseDragged:
                    mouseEvent = OFX_MAC_MOUSE_EVENT_LEFT_BUTTON_DRAGGED;
                    break;
                case kCGEventLeftMouseUp:
                    mouseEvent = OFX_MAC_MOUSE_EVENT_LEFT_BUTTON_UP;
                    break;
                case kCGEventRightMouseDown:
                    mouseEvent = OFX_MAC_MOUSE_EVENT_RIGHT_BUTTON_DOWN;
                    break;
                case kCGEventRightMouseDragged:
                    mouseEvent = OFX_MAC_MOUSE_EVENT_RIGHT_BUTTON_DRAGGED;
                    break;
                case kCGEventRightMouseUp:
                    mouseEvent = OFX_MAC_MOUSE_EVENT_RIGHT_BUTTON_UP;
                    break;
                case kCGEventOtherMouseDown:
                    mouseEvent = OFX_MAC_MOUSE_EVENT_OTHER_BUTTON_DOWN;
                    break;
                case kCGEventOtherMouseDragged:
                    mouseEvent = OFX_MAC_MOUSE_EVENT_OTHER_BUTTON_DRAGGED;
                    break;
                case kCGEventOtherMouseUp:
                    mouseEvent = OFX_MAC_MOUSE_EVENT_OTHER_BUTTON_UP;
                    break;
                case kCGEventMouseMoved:
                    mouseEvent = OFX_MAC_MOUSE_EVENT_MOVED;
                    break;
                case kCGEventScrollWheel:
                case kCGEventTabletPointer:
                case kCGEventTabletProximity:
                default:
                    return event;
            }
            CGPoint location = CGEventGetLocation(event);
            ofxMacMouseEventArg arg = (ofxMacMouseEventArg){
                .x = (float) location.x,
                .y = (float) location.y,
                .eventType = mouseEvent
            };
            ofNotifyEvent(ofxMacMouseEvent, arg);
            return event;
        }
    }
    
    CFMachPortRef mouseStealEvent = NULL;
    void ofxMacMouseStartStealMouseEvent() {
        if(mouseStealEvent) return;
        // The screen size of the primary display.
        CGEventMask eventMask   = CGEventMaskBit(kCGEventLeftMouseDown)
                                | CGEventMaskBit(kCGEventLeftMouseDragged)
                                | CGEventMaskBit(kCGEventLeftMouseUp)
                                | CGEventMaskBit(kCGEventRightMouseDown)
                                | CGEventMaskBit(kCGEventRightMouseDragged)
                                | CGEventMaskBit(kCGEventRightMouseUp)
                                | CGEventMaskBit(kCGEventOtherMouseDown)
                                | CGEventMaskBit(kCGEventOtherMouseDragged)
                                | CGEventMaskBit(kCGEventOtherMouseUp)
                                | CGEventMaskBit(kCGEventMouseMoved);
        mouseStealEvent = CGEventTapCreate(kCGSessionEventTap, kCGHeadInsertEventTap, kCGEventTapOptionDefault, eventMask, mouseStealer, NULL);
        if (!mouseStealEvent) {
            fprintf(stderr, "failed to create event tap\n");
        }
        
        CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, mouseStealEvent, 0);
        
        CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
        CGEventTapEnable(mouseStealEvent, true);
        CFRelease(runLoopSource);
    }
    
    void ofxMacMouseStopStealMouseEvent() {
        if(mouseStealEvent) {
            CGEventTapEnable(mouseStealEvent, false);
            CFRelease(mouseStealEvent);
            mouseStealEvent = NULL;
        }
    }
}
