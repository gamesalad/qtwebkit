/*
 *  Copyright (C) 2007 Holger Hans Peter Freyther zecke@selfish.org
 *            (C) 2009 Kenneth Rohde Christiansen
 *            (C) 2009 INdT, Instituto Nokia de Technologia
 *            (C) 2009-2010 ProFUSION embedded systems
 *            (C) 2009-2010 Samsung Electronics
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "config.h"
#include "ScrollbarEfl.h"

#include "CString.h"
#include "ChromeClient.h"
#include "Frame.h"
#include "FrameView.h"
#include "GraphicsContext.h"
#include "HostWindow.h"
#include "IntRect.h"
#include "NotImplemented.h"
#include "Page.h"
#include "ScrollbarTheme.h"

#include <Ecore.h>
#include <Edje.h>
#include <string>

using namespace std;
using namespace WebCore;

PassRefPtr<Scrollbar> Scrollbar::createNativeScrollbar(ScrollbarClient* client, ScrollbarOrientation orientation, ScrollbarControlSize size)
{
    return adoptRef(new ScrollbarEfl(client, orientation, size));
}

ScrollbarEfl::ScrollbarEfl(ScrollbarClient* client, ScrollbarOrientation orientation, ScrollbarControlSize controlSize)
    : Scrollbar(client, orientation, controlSize)
    , m_lastPos(0)
    , m_lastTotalSize(0)
    , m_lastVisibleSize(0)
{
    Widget::setFrameRect(IntRect(0, 0, 0, 0));
}

ScrollbarEfl::~ScrollbarEfl()
{
    if (!evasObject())
        return;
    evas_object_del(evasObject());
    setEvasObject(0);
}

static void scrollbarEflEdjeMessage(void* data, Evas_Object* o, Edje_Message_Type type, int id, void* msg)
{
    ScrollbarEfl::ScrollbarEfl* that = static_cast<ScrollbarEfl::ScrollbarEfl*>(data);
    Edje_Message_Float* m;
    int v;

    if (!id) {
        EINA_LOG_ERR("Unknown message id '%d' from scroll bar theme.", id);
        return;
    }

    if (type != EDJE_MESSAGE_FLOAT) {
        EINA_LOG_ERR("Message id '%d' of incorrect type from scroll bar theme. "
                     "Expected '%d', got '%d'.",
                     id, EDJE_MESSAGE_FLOAT, type);
        return;
    }

    m = static_cast<Edje_Message_Float*>(msg);
    v = m->val * (that->totalSize() - that->visibleSize());
    that->setValue(v);
}

void ScrollbarEfl::setParent(ScrollView* view)
{
    Evas_Object* o = evasObject();
    Evas_Coord w, h;

    Widget::setParent(view);

    if (!o) {
        if (!view)
            return;

        o = edje_object_add(view->evas());
        if (!o) {
            EINA_LOG_ERR("Could not create edje object for view=%p (evas=%p)",
                         view, view->evas());
            return;
        }
        edje_object_message_handler_set(o, scrollbarEflEdjeMessage, this);
        setEvasObject(o);
    } else if (!view) {
        evas_object_hide(o);
        return;
    }

    const char* group = (orientation() == HorizontalScrollbar)
        ? "scrollbar.horizontal" : "scrollbar.vertical";
    String theme(edjeThemeRecursive());

    if (theme.isEmpty()) {
        EINA_LOG_ERR("Could not load theme '%s': no theme path set.", group);
        evas_object_hide(o);
        return;
    }

    if (!edje_object_file_set(o, theme.utf8().data(), group)) {
        int err = edje_object_load_error_get(o);
        const char* errmsg = edje_load_error_str(err);
        EINA_LOG_ERR("Could not load theme '%s' from file '%s': #%d '%s'",
                     theme.utf8().data(), group, err, errmsg);
        return;
    }

    setPlatformWidget(o);
    evas_object_smart_member_add(o, view->evasObject());
    evas_object_show(o);

    edje_object_size_min_get(o, &w, &h);

    IntRect rect = frameRect();
    rect.setSize(IntSize(w, h));
    setFrameRect(rect);
}

void ScrollbarEfl::updateThumbPosition()
{
    updateThumbPositionAndProportion();
}

void ScrollbarEfl::updateThumbProportion()
{
    updateThumbPositionAndProportion();
}

void ScrollbarEfl::updateThumbPositionAndProportion()
{
    if (!platformWidget())
        return;

    int pos = currentPos();
    int tSize = totalSize();
    int vSize = visibleSize();

    if (m_lastPos == pos
        && m_lastTotalSize == tSize
        && m_lastVisibleSize == vSize)
        return;

    m_lastPos = pos;
    m_lastTotalSize = tSize;
    m_lastVisibleSize = vSize;

    Edje_Message_Float_Set* msg = static_cast<Edje_Message_Float_Set*>
        (alloca(sizeof(Edje_Message_Float_Set) + sizeof(float)));
    msg->count = 2;

    if (tSize - vSize > 0)
        msg->val[0] = pos / (float)(tSize - vSize);
    else
        msg->val[0] = 0.0;

    if (tSize > 0)
        msg->val[1] = vSize / (float)tSize;
    else
        msg->val[1] = 0.0;

    edje_object_message_send(platformWidget(), EDJE_MESSAGE_FLOAT_SET, 0, msg);
}

void ScrollbarEfl::setFrameRect(const IntRect& rect)
{
    Widget::setFrameRect(rect);
    frameRectsChanged();
}

void ScrollbarEfl::frameRectsChanged()
{
    Evas_Object* o = platformWidget();
    Evas_Coord x, y;

    if (!parent() || !o)
        return;

    IntRect rect = frameRect();
    if (parent()->isScrollViewScrollbar(this))
        rect.setLocation(parent()->convertToContainingWindow(rect.location()));
    else
        rect.setLocation(parent()->contentsToWindow(rect.location()));

    evas_object_geometry_get(root()->evasObject(), &x, &y, 0, 0);
    evas_object_move(o, x + rect.x(), y + rect.y());
    evas_object_resize(o, rect.width(), rect.height());
}

void ScrollbarEfl::paint(GraphicsContext* context, const IntRect& rect)
{
}

