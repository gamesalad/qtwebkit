/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// This file contains the definition for EventSender.
//
// Some notes about drag and drop handling:
// Windows drag and drop goes through a system call to doDragDrop.  At that
// point, program control is given to Windows which then periodically makes
// callbacks into the webview.  This won't work for layout tests, so instead,
// we queue up all the mouse move and mouse up events.  When the test tries to
// start a drag (by calling EvenSendingController::doDragDrop), we take the
// events in the queue and replay them.
// The behavior of queuing events and replaying them can be disabled by a
// layout test by setting eventSender.dragMode to false.

#include "config.h"
#include "EventSender.h"

#include "TestShell.h"
#include "base/compiler_specific.h"
#include "base/file_path.h"
#include "base/file_util.h"
#include "base/keyboard_codes.h"
#include "base/message_loop.h"
#include "base/string_util.h"
#include "base/sys_string_conversions.h"
#include "base/time.h"
#include "public/WebDragData.h"
#include "public/WebDragOperation.h"
#include "public/WebPoint.h"
#include "public/WebString.h"
#include "public/WebView.h"
#include <wtf/Deque.h>

#if OS(WINDOWS)
#include "public/win/WebInputEventFactory.h"
#endif

// FIXME: layout before each event?

using namespace base;
using namespace std;
using namespace WebKit;

TestShell* EventSender::testShell = 0;
WebPoint EventSender::lastMousePos;
WebMouseEvent::Button EventSender::pressedButton = WebMouseEvent::ButtonNone;
WebMouseEvent::Button EventSender::lastButtonType = WebMouseEvent::ButtonNone;

struct SavedEvent {
    enum SavedEventType {
        Unspecified,
        MouseUp,
        MouseMove,
        LeapForward
    };

    SavedEventType type;
    WebMouseEvent::Button buttonType; // For MouseUp.
    WebPoint pos; // For MouseMove.
    int milliseconds; // For LeapForward.

    SavedEvent()
        : type(Unspecified)
        , buttonType(WebMouseEvent::ButtonNone)
        , milliseconds(0) {}
};

static WebDragData currentDragData;
static WebDragOperation currentDragEffect;
static WebDragOperationsMask currentDragEffectsAllowed;
static bool replayingSavedEvents = false;
static Deque<SavedEvent> mouseEventQueue;

// Time and place of the last mouse up event.
static double lastClickTimeSec = 0;
static WebPoint lastClickPos;
static int clickCount = 0;

// maximum distance (in space and time) for a mouse click
// to register as a double or triple click
static const double multipleClickTimeSec = 1;
static const int multipleClickRadiusPixels = 5;

// How much we should scroll per event - the value here is chosen to
// match the WebKit impl and layout test results.
static const float scrollbarPixelsPerTick = 40.0f;

inline bool outsideMultiClickRadius(const WebPoint& a, const WebPoint& b)
{
    return ((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y)) >
        multipleClickRadiusPixels * multipleClickRadiusPixels;
}

// Used to offset the time the event hander things an event happened.  This is
// done so tests can run without a delay, but bypass checks that are time
// dependent (e.g., dragging has a timeout vs selection).
static uint32 timeOffsetMs = 0;

static double getCurrentEventTimeSec()
{
    return (TimeTicks::Now().ToInternalValue() / Time::kMicrosecondsPerMillisecond + timeOffsetMs) / 1000.0;
}

static void advanceEventTime(int32_t deltaMs)
{
    timeOffsetMs += deltaMs;
}

static void initMouseEvent(WebInputEvent::Type t, WebMouseEvent::Button b,
                           const gfx::Point& pos, WebMouseEvent* e)
{
    e->type = t;
    e->button = b;
    e->modifiers = 0;
    e->x = pos.x();
    e->y = pos.y();
    e->globalX = pos.x();
    e->globalY = pos.y();
    e->timeStampSeconds = getCurrentEventTimeSec();
    e->clickCount = clickCount;
}

// Returns true if the specified key is the system key.
static bool applyKeyModifier(const string& modifierName, WebInputEvent* event)
{
    bool isSystemKey = false;
    const char* characters = modifierName.c_str();
    if (!strcmp(characters, "ctrlKey")
#if !OS(MAC_OS_X)
        || !strcmp(characters, "addSelectionKey")
#endif
        ) {
        event->modifiers |= WebInputEvent::ControlKey;
    } else if (!strcmp(characters, "shiftKey") || !strcmp(characters, "rangeSelectionKey"))
        event->modifiers |= WebInputEvent::ShiftKey;
    else if (!strcmp(characters, "altKey")) {
        event->modifiers |= WebInputEvent::AltKey;
#if !OS(MAC_OS_X)
        // On Windows all keys with Alt modifier will be marked as system key.
        // We keep the same behavior on Linux, see:
        // WebKit/chromium/src/gtk/WebInputEventFactory.cpp
        // If we want to change this behavior on Linux, this piece of code must be
        // kept in sync with the related code in above file.
        isSystemKey = true;
#endif
#if OS(MAC_OS_X)
    } else if (!strcmp(characters, "metaKey") || !strcmp(characters, "addSelectionKey")) {
        event->modifiers |= WebInputEvent::MetaKey;
        // On Mac only command key presses are marked as system key.
        // See the related code in: WebKit/chromium/src/mac/WebInputEventFactory.cpp
        // It must be kept in sync with the related code in above file.
        isSystemKey = true;
#else
    } else if (!wcscmp(characters, L"metaKey")) {
        event->modifiers |= WebInputEvent::MetaKey;
#endif
    }
    return isSystemKey;
}

static bool applyKeyModifiers(const CppVariant* argument, WebInputEvent* event)
{
    bool isSystemKey = false;
    if (argument->isObject()) {
        Vector<string> modifiers = argument->toStringVector();
        for (Vector<string>::const_iterator i = modifiers.begin(); i != modifiers.end(); ++i)
            isSystemKey |= applyKeyModifier(*i, event);
    } else if (argument->isString())
        isSystemKey = applyKeyModifier(argument->toString(), event);
    return isSystemKey;
}

// Get the edit command corresponding to a keyboard event.
// Returns true if the specified event corresponds to an edit command, the name
// of the edit command will be stored in |*name|.
bool getEditCommand(const WebKeyboardEvent& event, string* name)
{
#if OS(MAC_OS_X)
    // We only cares about Left,Right,Up,Down keys with Command or Command+Shift
    // modifiers. These key events correspond to some special movement and
    // selection editor commands, and was supposed to be handled in
    // WebKit/chromium/src/EditorClientImpl.cpp. But these keys will be marked
    // as system key, which prevents them from being handled. Thus they must be
    // handled specially.
    if ((event.modifiers & ~WebKeyboardEvent::ShiftKey) != WebKeyboardEvent::MetaKey)
        return false;

    switch (event.windowsKeyCode) {
    case base::VKEY_LEFT:
        *name = "MoveToBeginningOfLine";
        break;
    case base::VKEY_RIGHT:
        *name = "MoveToEndOfLine";
        break;
    case base::VKEY_UP:
        *name = "MoveToBeginningOfDocument";
        break;
    case base::VKEY_DOWN:
        *name = "MoveToEndOfDocument";
        break;
    default:
        return false;
    }

    if (event.modifiers & WebKeyboardEvent::ShiftKey)
        name->append("AndModifySelection");

    return true;
#else
    return false;
#endif
}

// Key event location code introduced in DOM Level 3.
// See also: http://www.w3.org/TR/DOM-Level-3-Events/#events-keyboardevents
enum KeyLocationCode {
    DOMKeyLocationStandard      = 0x00,
    DOMKeyLocationLeft          = 0x01,
    DOMKeyLocationRight         = 0x02,
    DOMKeyLocationNumpad        = 0x03
};

EventSender::EventSender(TestShell* shell)
    : ALLOW_THIS_IN_INITIALIZER_LIST(m_methodFactory(this))
{
    // Set static testShell variable since we can't do it in an initializer list.
    // We also need to be careful not to assign testShell to new windows which are
    // temporary.
    if (!testShell)
        testShell = shell;

    // Initialize the map that associates methods of this class with the names
    // they will use when called by JavaScript.  The actual binding of those
    // names to their methods will be done by calling bindToJavaScript() (defined
    // by CppBoundClass, the parent to EventSender).
    bindMethod("mouseDown", &EventSender::mouseDown);
    bindMethod("mouseUp", &EventSender::mouseUp);
    bindMethod("contextClick", &EventSender::contextClick);
    bindMethod("mouseMoveTo", &EventSender::mouseMoveTo);
    bindMethod("mouseWheelTo", &EventSender::mouseWheelTo);
    bindMethod("leapForward", &EventSender::leapForward);
    bindMethod("keyDown", &EventSender::keyDown);
    bindMethod("dispatchMessage", &EventSender::dispatchMessage);
    bindMethod("enableDOMUIEventLogging", &EventSender::enableDOMUIEventLogging);
    bindMethod("fireKeyboardEventsToElement", &EventSender::fireKeyboardEventsToElement);
    bindMethod("clearKillRing", &EventSender::clearKillRing);
    bindMethod("textZoomIn", &EventSender::textZoomIn);
    bindMethod("textZoomOut", &EventSender::textZoomOut);
    bindMethod("zoomPageIn", &EventSender::zoomPageIn);
    bindMethod("zoomPageOut", &EventSender::zoomPageOut);
    bindMethod("scheduleAsynchronousClick", &EventSender::scheduleAsynchronousClick);
    bindMethod("beginDragWithFiles", &EventSender::beginDragWithFiles);

    // When set to true (the default value), we batch mouse move and mouse up
    // events so we can simulate drag & drop.
    bindProperty("dragMode", &dragMode);
#if OS(WINDOWS)
    bindProperty("WM_KEYDOWN", &wmKeyDown);
    bindProperty("WM_KEYUP", &wmKeyUp);
    bindProperty("WM_CHAR", &wmChar);
    bindProperty("WM_DEADCHAR", &wmDeadChar);
    bindProperty("WM_SYSKEYDOWN", &wmSysKeyDown);
    bindProperty("WM_SYSKEYUP", &wmSysKeyUp);
    bindProperty("WM_SYSCHAR", &wmSysChar);
    bindProperty("WM_SYSDEADCHAR", &wmSysDeadChar);
#endif
}

void EventSender::reset()
{
    // The test should have finished a drag and the mouse button state.
    ASSERT(currentDragData.isNull());
    currentDragData.reset();
    currentDragEffect = WebKit::WebDragOperationNone;
    currentDragEffectsAllowed = WebKit::WebDragOperationNone;
    pressedButton = WebMouseEvent::ButtonNone;
    dragMode.set(true);
#if OS(WINDOWS)
    wmKeyDown.Set(WM_KEYDOWN);
    wmKeyUp.Set(WM_KEYUP);
    wmChar.Set(WM_CHAR);
    wmDeadChar.Set(WM_DEADCHAR);
    wmSysKeyDown.Set(WM_SYSKEYDOWN);
    wmSysKeyUp.Set(WM_SYSKEYUP);
    wmSysChar.Set(WM_SYSCHAR);
    wmSysDeadChar.Set(WM_SYSDEADCHAR);
#endif
    lastMousePos = WebPoint(0, 0);
    lastClickTimeSec = 0;
    lastClickPos = WebPoint(0, 0);
    clickCount = 0;
    lastButtonType = WebMouseEvent::ButtonNone;
    timeOffsetMs = 0;
}

WebView* EventSender::webview()
{
    return testShell->webView();
}

void EventSender::doDragDrop(const WebKit::WebPoint& eventPos,
                             const WebDragData& dragData,
                             WebDragOperationsMask mask)
{
    WebMouseEvent event;
    initMouseEvent(WebInputEvent::MouseDown, pressedButton, lastMousePos, &event);
    WebPoint clientPoint(event.x, event.y);
    WebPoint screenPoint(event.globalX, event.globalY);
    currentDragData = dragData;
    currentDragEffectsAllowed = mask;
    currentDragEffect = webview()->dragTargetDragEnter(dragData, 0, clientPoint, screenPoint, currentDragEffectsAllowed);

    // Finish processing events.
    replaySavedEvents();
}

WebMouseEvent::Button EventSender::getButtonTypeFromButtonNumber(int buttonCode)
{
    if (!buttonCode)
        return WebMouseEvent::ButtonLeft;
    if (buttonCode == 2)
        return WebMouseEvent::ButtonRight;
    return WebMouseEvent::ButtonMiddle;
}

int EventSender::getButtonNumberFromSingleArg(const CppArgumentList& arguments)
{
    int buttonCode = 0;
    if (arguments.size() > 0 && arguments[0].isNumber())
        buttonCode = arguments[0].toInt32();
    return buttonCode;
}

void EventSender::updateClickCountForButton(WebMouseEvent::Button buttonType)
{
    if ((getCurrentEventTimeSec() - lastClickTimeSec < multipleClickTimeSec)
        && (!outsideMultiClickRadius(lastMousePos, lastClickPos))
        && (buttonType == lastButtonType))
        ++clickCount;
    else {
        clickCount = 1;
        lastButtonType = buttonType;
    }
}

//
// Implemented javascript methods.
//

void EventSender::mouseDown(const CppArgumentList& arguments, CppVariant* result)
{
    if (result) // Could be 0 if invoked asynchronously.
        result->setNull();

    webview()->layout();

    int buttonNumber = getButtonNumberFromSingleArg(arguments);
    ASSERT(buttonNumber != -1);

    WebMouseEvent::Button buttonType = getButtonTypeFromButtonNumber(buttonNumber);

    updateClickCountForButton(buttonType);

    WebMouseEvent event;
    pressedButton = buttonType;
    initMouseEvent(WebInputEvent::MouseDown, buttonType, lastMousePos, &event);
    if (arguments.size() >= 2 && (arguments[1].isObject() || arguments[1].isString()))
        applyKeyModifiers(&(arguments[1]), &event);
    webview()->handleInputEvent(event);
}

void EventSender::mouseUp(const CppArgumentList& arguments, CppVariant* result)
{
    if (result) // Could be 0 if invoked asynchronously.
        result->setNull();

    webview()->layout();

    int buttonNumber = getButtonNumberFromSingleArg(arguments);
    ASSERT(buttonNumber != -1);

    WebMouseEvent::Button buttonType = getButtonTypeFromButtonNumber(buttonNumber);

    if (isDragMode() && !replayingSavedEvents) {
        SavedEvent savedEvent;
        savedEvent.type = SavedEvent::MouseUp;
        savedEvent.buttonType = buttonType;
        mouseEventQueue.append(savedEvent);
        replaySavedEvents();
    } else {
        WebMouseEvent event;
        initMouseEvent(WebInputEvent::MouseUp, buttonType, lastMousePos, &event);
        if (arguments.size() >= 2 && (arguments[1].isObject() || arguments[1].isString()))
            applyKeyModifiers(&(arguments[1]), &event);
        doMouseUp(event);
    }
}

void EventSender::doMouseUp(const WebMouseEvent& e)
{
    webview()->handleInputEvent(e);

    pressedButton = WebMouseEvent::ButtonNone;
    lastClickTimeSec = e.timeStampSeconds;
    lastClickPos = lastMousePos;

    // If we're in a drag operation, complete it.
    if (currentDragData.isNull())
        return;
    WebPoint clientPoint(e.x, e.y);
    WebPoint screenPoint(e.globalX, e.globalY);

    currentDragEffect = webview()->dragTargetDragOver(clientPoint, screenPoint, currentDragEffectsAllowed);
    if (currentDragEffect)
        webview()->dragTargetDrop(clientPoint, screenPoint);
    else
        webview()->dragTargetDragLeave();
    webview()->dragSourceEndedAt(clientPoint, screenPoint, currentDragEffect);
    webview()->dragSourceSystemDragEnded();

    currentDragData.reset();
}

void EventSender::mouseMoveTo(const CppArgumentList& arguments, CppVariant* result)
{
    result->setNull();

    if (arguments.size() < 2 || !arguments[0].isNumber() || !arguments[1].isNumber())
        return;
    webview()->layout();

    WebPoint mousePos(arguments[0].toInt32(), arguments[1].toInt32());

    if (isDragMode() && pressedButton == WebMouseEvent::ButtonLeft && !replayingSavedEvents) {
        SavedEvent savedEvent;
        savedEvent.type = SavedEvent::MouseMove;
        savedEvent.pos = mousePos;
        mouseEventQueue.append(savedEvent);
    } else {
        WebMouseEvent event;
        initMouseEvent(WebInputEvent::MouseMove, pressedButton, mousePos, &event);
        doMouseMove(event);
    }
}

void EventSender::mouseWheelTo(const CppArgumentList& arguments, CppVariant* result)
{
    result->setNull();

    if (arguments.size() < 2 || !arguments[0].isNumber() || !arguments[1].isNumber())
        return;

    // Force a layout here just to make sure every position has been
    // determined before we send events (as well as all the other methods
    // that send an event do). The layout test calling this
    // (scrollbars/overflow-scrollbar-horizontal-wheel-scroll.html, only one
    // for now) does not rely on this though.
    webview()->layout();

    int horizontal = arguments[0].toInt32();
    int vertical = arguments[1].toInt32();

    WebMouseWheelEvent event;
    initMouseEvent(WebInputEvent::MouseWheel, pressedButton, lastMousePos, &event);
    event.wheelTicksX = static_cast<float>(horizontal);
    event.wheelTicksY = static_cast<float>(vertical);
    event.deltaX = -horizontal * scrollbarPixelsPerTick;
    event.deltaY = -vertical * scrollbarPixelsPerTick;
    webview()->handleInputEvent(event);
}

void EventSender::doMouseMove(const WebMouseEvent& e)
{
    lastMousePos = WebPoint(e.x, e.y);

    webview()->handleInputEvent(e);

    if (pressedButton == WebMouseEvent::ButtonNone || currentDragData.isNull())
        return;
    WebPoint clientPoint(e.x, e.y);
    WebPoint screenPoint(e.globalX, e.globalY);
    currentDragEffect = webview()->dragTargetDragOver(clientPoint, screenPoint, currentDragEffectsAllowed);
}

void EventSender::keyDown(const CppArgumentList& arguments, CppVariant* result)
{
    result->setNull();
    if (arguments.size() < 1 || !arguments[0].isString())
        return;
    bool generateChar = false;

    // FIXME: I'm not exactly sure how we should convert the string to a key
    // event. This seems to work in the cases I tested.
    // FIXME: Should we also generate a KEY_UP?
    wstring codeStr = UTF8ToWide(arguments[0].toString());

    // Convert \n -> VK_RETURN.  Some layout tests use \n to mean "Enter", when
    // Windows uses \r for "Enter".
    int code = 0;
    int text = 0;
    bool needsShiftKeyModifier = false;
    if (L"\n" == codeStr) {
        generateChar = true;
        text = code = base::VKEY_RETURN;
    } else if (L"rightArrow" == codeStr)
        code = base::VKEY_RIGHT;
    else if (L"downArrow" == codeStr)
        code = base::VKEY_DOWN;
    else if (L"leftArrow" == codeStr)
        code = base::VKEY_LEFT;
    else if (L"upArrow" == codeStr)
        code = base::VKEY_UP;
    else if (L"delete" == codeStr)
        code = base::VKEY_BACK;
    else if (L"pageUp" == codeStr)
        code = base::VKEY_PRIOR;
    else if (L"pageDown" == codeStr)
        code = base::VKEY_NEXT;
    else if (L"home" == codeStr)
        code = base::VKEY_HOME;
    else if (L"end" == codeStr)
        code = base::VKEY_END;
    else {
        // Compare the input string with the function-key names defined by the
        // DOM spec (i.e. "F1",...,"F24"). If the input string is a function-key
        // name, set its key code.
        for (int i = 1; i <= 24; ++i) {
            wstring functionKeyName;
            functionKeyName += L"F";
            functionKeyName += IntToWString(i);
            if (functionKeyName == codeStr) {
                code = base::VKEY_F1 + (i - 1);
                break;
            }
        }
        if (!code) {
            ASSERT(codeStr.length() == 1);
            text = code = codeStr[0];
            needsShiftKeyModifier = needsShiftModifier(code);
            if ((code & 0xFF) >= 'a' && (code & 0xFF) <= 'z')
                code -= 'a' - 'A';
            generateChar = true;
        }
    }

    // For one generated keyboard event, we need to generate a keyDown/keyUp
    // pair; refer to EventSender.cpp in WebKit/WebKitTools/DumpRenderTree/win.
    // On Windows, we might also need to generate a char event to mimic the
    // Windows event flow; on other platforms we create a merged event and test
    // the event flow that that platform provides.
    WebKeyboardEvent eventDown, eventChar, eventUp;
    eventDown.type = WebInputEvent::RawKeyDown;
    eventDown.modifiers = 0;
    eventDown.windowsKeyCode = code;
    if (generateChar) {
        eventDown.text[0] = text;
        eventDown.unmodifiedText[0] = text;
    }
    eventDown.setKeyIdentifierFromWindowsKeyCode();

    if (arguments.size() >= 2 && (arguments[1].isObject() || arguments[1].isString()))
        eventDown.isSystemKey = applyKeyModifiers(&(arguments[1]), &eventDown);

    if (needsShiftKeyModifier)
        eventDown.modifiers |= WebInputEvent::ShiftKey;

    // See if KeyLocation argument is given.
    if (arguments.size() >= 3 && arguments[2].isNumber()) {
        int location = arguments[2].toInt32();
        if (location == DOMKeyLocationNumpad)
            eventDown.modifiers |= WebInputEvent::IsKeyPad;
    }

    eventChar = eventUp = eventDown;
    eventUp.type = WebInputEvent::KeyUp;
    // EventSender.m forces a layout here, with at least one
    // test (fast/forms/focus-control-to-page.html) relying on this.
    webview()->layout();

    // In the browser, if a keyboard event corresponds to an editor command,
    // the command will be dispatched to the renderer just before dispatching
    // the keyboard event, and then it will be executed in the
    // RenderView::handleCurrentKeyboardEvent() method, which is called from
    // third_party/WebKit/WebKit/chromium/src/EditorClientImpl.cpp.
    // We just simulate the same behavior here.
    string editCommand;
    if (getEditCommand(eventDown, &editCommand))
        testShell->delegate()->setEditCommand(editCommand, "");

    webview()->handleInputEvent(eventDown);

    testShell->delegate()->clearEditCommand();

    if (generateChar) {
        eventChar.type = WebInputEvent::Char;
        eventChar.keyIdentifier[0] = '\0';
        webview()->handleInputEvent(eventChar);
    }

    webview()->handleInputEvent(eventUp);
}

void EventSender::dispatchMessage(const CppArgumentList& arguments, CppVariant* result)
{
    result->setNull();

#if OS(WINDOWS)
    if (arguments.size() == 3) {
        // Grab the message id to see if we need to dispatch it.
        int msg = arguments[0].ToInt32();

        // WebKit's version of this function stuffs a MSG struct and uses
        // TranslateMessage and DispatchMessage. We use a WebKeyboardEvent, which
        // doesn't need to receive the DeadChar and SysDeadChar messages.
        if (msg == WM_DEADCHAR || msg == WM_SYSDEADCHAR)
            return;

        webview()->layout();

        unsigned long lparam = static_cast<unsigned long>(arguments[2].ToDouble());
        webview()->handleInputEvent(WebInputEventFactory::keyboardEvent(0, msg, arguments[1].ToInt32(), lparam));
    } else
        ASSERT_NOT_REACHED();
#endif
}

bool EventSender::needsShiftModifier(int keyCode)
{
    // If code is an uppercase letter, assign a SHIFT key to
    // eventDown.modifier, this logic comes from
    // WebKit/WebKitTools/DumpRenderTree/Win/EventSender.cpp
    return (keyCode & 0xFF) >= 'A' && (keyCode & 0xFF) <= 'Z';
}

void EventSender::leapForward(const CppArgumentList& arguments, CppVariant* result)
{
    result->setNull();

    if (arguments.size() < 1 || !arguments[0].isNumber())
        return;

    int milliseconds = arguments[0].toInt32();
    if (isDragMode() && pressedButton == WebMouseEvent::ButtonLeft && !replayingSavedEvents) {
        SavedEvent savedEvent;
        savedEvent.type = SavedEvent::LeapForward;
        savedEvent.milliseconds = milliseconds;
        mouseEventQueue.append(savedEvent);
    } else
        doLeapForward(milliseconds);
}

void EventSender::doLeapForward(int milliseconds)
{
    advanceEventTime(milliseconds);
}

// Apple's port of WebKit zooms by a factor of 1.2 (see
// WebKit/WebView/WebView.mm)
void EventSender::textZoomIn(const CppArgumentList&, CppVariant* result)
{
    webview()->setZoomLevel(true, webview()->zoomLevel() + 1);
    result->setNull();
}

void EventSender::textZoomOut(const CppArgumentList&, CppVariant* result)
{
    webview()->setZoomLevel(true, webview()->zoomLevel() - 1);
    result->setNull();
}

void EventSender::zoomPageIn(const CppArgumentList&, CppVariant* result)
{
    webview()->setZoomLevel(false, webview()->zoomLevel() + 1);
    result->setNull();
}

void EventSender::zoomPageOut(const CppArgumentList&, CppVariant* result)
{
    webview()->setZoomLevel(false, webview()->zoomLevel() - 1);
    result->setNull();
}

void EventSender::replaySavedEvents()
{
    replayingSavedEvents = true;
    while (!mouseEventQueue.isEmpty()) {
        SavedEvent e = mouseEventQueue.first();
        mouseEventQueue.removeFirst();

        switch (e.type) {
        case SavedEvent::MouseMove: {
            WebMouseEvent event;
            initMouseEvent(WebInputEvent::MouseMove, pressedButton, e.pos, &event);
            doMouseMove(event);
            break;
        }
        case SavedEvent::LeapForward:
            doLeapForward(e.milliseconds);
            break;
        case SavedEvent::MouseUp: {
            WebMouseEvent event;
            initMouseEvent(WebInputEvent::MouseUp, e.buttonType, lastMousePos, &event);
            doMouseUp(event);
            break;
        }
        default:
            ASSERT_NOT_REACHED();
        }
    }

    replayingSavedEvents = false;
}

void EventSender::contextClick(const CppArgumentList& arguments, CppVariant* result)
{
    result->setNull();

    webview()->layout();

    updateClickCountForButton(WebMouseEvent::ButtonRight);

    // Generate right mouse down and up.

    WebMouseEvent event;
    pressedButton = WebMouseEvent::ButtonRight;
    initMouseEvent(WebInputEvent::MouseDown, WebMouseEvent::ButtonRight, lastMousePos, &event);
    webview()->handleInputEvent(event);

    initMouseEvent(WebInputEvent::MouseUp, WebMouseEvent::ButtonRight, lastMousePos, &event);
    webview()->handleInputEvent(event);

    pressedButton = WebMouseEvent::ButtonNone;
}

void EventSender::scheduleAsynchronousClick(const CppArgumentList& arguments, CppVariant* result)
{
    result->setNull();

    MessageLoop::current()->PostTask(
        FROM_HERE, m_methodFactory.NewRunnableMethod(
            &EventSender::mouseDown, arguments, static_cast<CppVariant*>(0)));
    MessageLoop::current()->PostTask(
        FROM_HERE, m_methodFactory.NewRunnableMethod(
            &EventSender::mouseUp, arguments, static_cast<CppVariant*>(0)));
}

static WebString filePathStringToWebString(const FilePath::StringType& pathString)
{
#if OS(UNIX)
    return WideToUTF16Hack(SysNativeMBToWide(pathString));
#elif OS(WINDOWS)
    return WebString(pathString);
#endif
}

void EventSender::beginDragWithFiles(const CppArgumentList& arguments, CppVariant* result)
{
    currentDragData.initialize();
    Vector<string> files = arguments[0].toStringVector();
    for (size_t i = 0; i < files.size(); ++i) {
        FilePath path(files[i]);
        file_util::AbsolutePath(&path);
        currentDragData.appendToFileNames(filePathStringToWebString(path.value()));
    }
    currentDragEffectsAllowed = WebKit::WebDragOperationCopy;

    // Provide a drag source.
    webview()->dragTargetDragEnter(currentDragData, 0, lastMousePos, lastMousePos, currentDragEffectsAllowed);

    // dragMode saves events and then replays them later. We don't need/want that.
    dragMode.set(false);

    // Make the rest of eventSender think a drag is in progress.
    pressedButton = WebMouseEvent::ButtonLeft;

    result->setNull();
}

//
// Unimplemented stubs
//

void EventSender::enableDOMUIEventLogging(const CppArgumentList&, CppVariant* result)
{
    result->setNull();
}

void EventSender::fireKeyboardEventsToElement(const CppArgumentList&, CppVariant* result)
{
    result->setNull();
}

void EventSender::clearKillRing(const CppArgumentList&, CppVariant* result)
{
    result->setNull();
}
