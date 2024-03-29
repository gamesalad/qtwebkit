/*
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2008 Kenneth Rohde Christiansen
 * Copyright (C) 2008 Diego Gonzalez
 * Copyright (C) 2009-2010 ProFUSION embedded systems
 * Copyright (C) 2009-2010 Samsung Electronics
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "ChromeClientEfl.h"

#include "CString.h"
#include "DatabaseTracker.h"
#include "EWebKit.h"
#include "FloatRect.h"
#include "FrameLoader.h"
#include "FrameLoaderClientEfl.h"
#include "HitTestResult.h"
#include "IntRect.h"
#include "KURL.h"
#include "NotImplemented.h"
#include "PlatformString.h"
#include "WindowFeatures.h"
#include "ewk_private.h"

using namespace WebCore;

static inline Evas_Object* kit(WebCore::Frame* frame)
{
    if (!frame)
        return 0;

    FrameLoaderClientEfl* client = static_cast<FrameLoaderClientEfl*>(frame->loader()->client());
    return client ? client->webFrame() : 0;
}

namespace WebCore {

ChromeClientEfl::ChromeClientEfl(Evas_Object *view)
    : m_view(view)
{
}

ChromeClientEfl::~ChromeClientEfl()
{
}

void ChromeClientEfl::chromeDestroyed()
{
    delete this;
}

void ChromeClientEfl::focusedNodeChanged(Node*)
{
    notImplemented();
}

FloatRect ChromeClientEfl::windowRect()
{
    notImplemented();
    return FloatRect();
}

void ChromeClientEfl::setWindowRect(const FloatRect& rect)
{
    notImplemented();
}

FloatRect ChromeClientEfl::pageRect()
{
    notImplemented();
    return FloatRect();
}

float ChromeClientEfl::scaleFactor()
{
    notImplemented();
    return 1.0;
}

void ChromeClientEfl::focus()
{
    evas_object_focus_set(m_view, EINA_TRUE);
}

void ChromeClientEfl::unfocus()
{
    evas_object_focus_set(m_view, EINA_FALSE);
}

Page* ChromeClientEfl::createWindow(Frame*, const FrameLoadRequest& request, const WindowFeatures& features)
{
    notImplemented();
    return 0;
}

void ChromeClientEfl::show()
{
    ewk_view_ready(m_view);
}

bool ChromeClientEfl::canRunModal()
{
    notImplemented();
    return false;
}

void ChromeClientEfl::runModal()
{
    notImplemented();
}

void ChromeClientEfl::setToolbarsVisible(bool visible)
{
    ewk_view_toolbars_visible_set(m_view, visible);
}

bool ChromeClientEfl::toolbarsVisible()
{
    Eina_Bool visible;

    ewk_view_toolbars_visible_get(m_view, &visible);
    return visible;
}

void ChromeClientEfl::setStatusbarVisible(bool visible)
{
    ewk_view_statusbar_visible_set(m_view, visible);
}

bool ChromeClientEfl::statusbarVisible()
{
    Eina_Bool visible;

    ewk_view_statusbar_visible_get(m_view, &visible);
    return visible;
}

void ChromeClientEfl::setScrollbarsVisible(bool visible)
{
    ewk_view_scrollbars_visible_set(m_view, visible);
}

bool ChromeClientEfl::scrollbarsVisible()
{
    Eina_Bool visible;

    ewk_view_scrollbars_visible_get(m_view, &visible);
    return visible;
}

void ChromeClientEfl::setMenubarVisible(bool visible)
{
    ewk_view_menubar_visible_set(m_view, visible);
}

bool ChromeClientEfl::menubarVisible()
{
    Eina_Bool visible;

    ewk_view_menubar_visible_get(m_view, &visible);
    return visible;
}

void ChromeClientEfl::setResizable(bool)
{
    notImplemented();
}

void ChromeClientEfl::closeWindowSoon()
{
    ewk_view_stop(m_view);
}

bool ChromeClientEfl::canTakeFocus(FocusDirection)
{
    // This is called when cycling through links/focusable objects and we
    // reach the last focusable object.
    return false;
}

void ChromeClientEfl::takeFocus(FocusDirection)
{
    unfocus();
}

bool ChromeClientEfl::canRunBeforeUnloadConfirmPanel()
{
    return true;
}

bool ChromeClientEfl::runBeforeUnloadConfirmPanel(const String& message, WebCore::Frame* frame)
{
    return runJavaScriptConfirm(frame, message);
}

void ChromeClientEfl::addMessageToConsole(MessageSource, MessageType, MessageLevel, const String& message,
                                          unsigned int lineNumber, const String& sourceID)
{
    ewk_view_add_console_message(m_view, message.utf8().data(), lineNumber, sourceID.utf8().data());
}

void ChromeClientEfl::runJavaScriptAlert(Frame* frame, const String& message)
{
    ewk_view_run_javascript_alert(m_view, kit(frame), message.utf8().data());
}

bool ChromeClientEfl::runJavaScriptConfirm(Frame* frame, const String& message)
{
    return ewk_view_run_javascript_confirm(m_view, kit(frame), message.utf8().data());
}

bool ChromeClientEfl::runJavaScriptPrompt(Frame* frame, const String& message, const String& defaultValue, String& result)
{
    char* value = 0;
    ewk_view_run_javascript_prompt(m_view, kit(frame), message.utf8().data(), defaultValue.utf8().data(), &value);
    if (value) {
        result = String::fromUTF8(value);
        free(value);
        return true;
    }
    return false;
}

void ChromeClientEfl::setStatusbarText(const String& string)
{
    ewk_view_statusbar_text_set(m_view, string.utf8().data());
}

bool ChromeClientEfl::shouldInterruptJavaScript()
{
    return ewk_view_should_interrupt_javascript(m_view);
}

bool ChromeClientEfl::tabsToLinks() const
{
    return true;
}

IntRect ChromeClientEfl::windowResizerRect() const
{
    notImplemented();
    // Implementing this function will make repaint being
    // called during resize, but as this will be done with
    // a minor delay it adds a weird "filling" effect due
    // to us using an evas image for showing the cairo
    // context. So instead of implementing this function
    // we call paint directly during resize with
    // the new object size as its argument.
    return IntRect();
}

void ChromeClientEfl::repaint(const IntRect& windowRect, bool contentChanged, bool immediate, bool repaintContentOnly)
{
    Evas_Coord x, y, w, h;

    if (!contentChanged)
        return;

    x = windowRect.x();
    y = windowRect.y();
    w = windowRect.width();
    h = windowRect.height();
    ewk_view_repaint(m_view, x, y, w, h);
}

void ChromeClientEfl::contentsSizeChanged(Frame* frame, const IntSize& size) const
{
    ewk_frame_contents_size_changed(kit(frame), size.width(), size.height());
}

bool ChromeClientEfl::scroll(const IntSize& delta, const IntRect& scrollViewRect, const IntRect& clipRect, bool canBlit, bool isMainFrame)
{
    ewk_view_scroll(m_view, delta.width(), delta.height(), scrollViewRect.x(), scrollViewRect.y(), scrollViewRect.width(), scrollViewRect.height(), clipRect.x(), clipRect.y(), clipRect.width(), clipRect.height(), isMainFrame);
    return canBlit;
}

IntRect ChromeClientEfl::windowToScreen(const IntRect& rect) const
{
    notImplemented();
    return rect;
}

IntPoint ChromeClientEfl::screenToWindow(const IntPoint& point) const
{
    notImplemented();
    return point;
}

PlatformPageClient ChromeClientEfl::platformPageClient() const
{
    return m_view;
}

void ChromeClientEfl::scrollbarsModeDidChange() const
{
}

void ChromeClientEfl::mouseDidMoveOverElement(const HitTestResult& hit, unsigned modifierFlags)
{
    // FIXME, compare with old link, look at Qt impl.
    bool isLink = hit.isLiveLink();
    if (isLink) {
        KURL url = hit.absoluteLinkURL();
        if (!url.isEmpty() && url != m_hoveredLinkURL) {
            const char *link[2];
            TextDirection dir;
            CString urlStr = url.prettyURL().utf8();
            CString titleStr = hit.title(dir).utf8();
            link[0] = urlStr.data();
            link[1] = titleStr.data();
            ewk_view_mouse_link_hover_in(m_view, link);
            m_hoveredLinkURL = url;
        }
    } else if (!isLink && !m_hoveredLinkURL.isEmpty()) {
        ewk_view_mouse_link_hover_out(m_view);
        m_hoveredLinkURL = KURL();
    }
}

void ChromeClientEfl::setToolTip(const String& toolTip, TextDirection)
{
    ewk_view_tooltip_text_set(m_view, toolTip.utf8().data());
}

void ChromeClientEfl::print(Frame* frame)
{
    notImplemented();
}

#if ENABLE(OFFLINE_WEB_APPLICATIONS)
void ChromeClientEfl::reachedMaxAppCacheSize(int64_t spaceNeeded)
{
    // FIXME: Free some space.
    notImplemented();
}
#endif

void ChromeClientEfl::exceededDatabaseQuota(Frame* frame, const String& databaseName)
{
    uint64_t quota = ewk_settings_web_database_default_quota_get();

    if (!DatabaseTracker::tracker().hasEntryForOrigin(frame->document()->securityOrigin()))
        DatabaseTracker::tracker().setQuota(frame->document()->securityOrigin(), quota);

    ewk_view_exceeded_database_quota(m_view, kit(frame), databaseName.utf8().data());
}

void ChromeClientEfl::runOpenPanel(Frame*, PassRefPtr<FileChooser> prpFileChooser)
{
    notImplemented();
}

void ChromeClientEfl::formStateDidChange(const Node*)
{
    notImplemented();
}

bool ChromeClientEfl::setCursor(PlatformCursorHandle)
{
    notImplemented();
    return false;
}

void ChromeClientEfl::requestGeolocationPermissionForFrame(Frame*, Geolocation*)
{
    // See the comment in WebCore/page/ChromeClient.h
    notImplemented();
}

}
