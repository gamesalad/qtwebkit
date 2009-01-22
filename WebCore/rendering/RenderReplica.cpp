/*
 * Copyright (C) 2008 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "RenderReplica.h"

#include "RenderLayer.h"

namespace WebCore {

RenderReplica::RenderReplica(Node* n)
: RenderBox(n)
{}

RenderReplica::~RenderReplica()
{}
    
void RenderReplica::layout()
{
    m_frameRect = parentBox()->borderBoxRect();
    setNeedsLayout(false);
}

void RenderReplica::calcPrefWidths()
{
    m_minPrefWidth = parentBox()->width();
    m_maxPrefWidth = m_minPrefWidth;
    setPrefWidthsDirty(false);
}

void RenderReplica::paint(PaintInfo& paintInfo, int tx, int ty)
{
    if (paintInfo.phase != PaintPhaseForeground && paintInfo.phase != PaintPhaseMask)
        return;
 
    tx += x();
    ty += m_frameRect.y();

    if (paintInfo.phase == PaintPhaseForeground)
        // Turn around and paint the parent layer. Use temporary clipRects, so that the layer doesn't end up caching clip rects
        // computing using the wrong rootLayer
        layer()->parent()->paintLayer(layer()->transform() ? layer()->parent() : layer()->enclosingTransformedAncestor(),
                                      paintInfo.context, paintInfo.rect,
                                      true, PaintRestrictionNone, 0,
                                      true,     // appliedTransform
                                      true);    // temporaryClipRects
    else if (paintInfo.phase == PaintPhaseMask)
        paintMask(paintInfo, tx, ty);
}

} // namespace WebCore
