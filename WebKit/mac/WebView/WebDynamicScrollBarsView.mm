/*
 * Copyright (C) 2005, 2008, 2010 Apple Inc. All rights reserved.
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

#import "WebDynamicScrollBarsViewInternal.h"

#import "WebDocument.h"
#import "WebFrameInternal.h"
#import "WebFrameView.h"
#import "WebHTMLViewInternal.h"
#import <WebCore/Frame.h>
#import <WebCore/FrameView.h>
#import <WebKitSystemInterface.h>

using namespace WebCore;

// FIXME: <rdar://problem/5898985> Mail expects a constant of this name to exist.
const int WebCoreScrollbarAlwaysOn = ScrollbarAlwaysOn;

@implementation WebDynamicScrollBarsView

- (void)setAllowsHorizontalScrolling:(BOOL)flag
{
    if (hScrollModeLocked)
        return;
    if (flag && hScroll == ScrollbarAlwaysOff)
        hScroll = ScrollbarAuto;
    else if (!flag && hScroll != ScrollbarAlwaysOff)
        hScroll = ScrollbarAlwaysOff;
    [self updateScrollers];
}

- (void)setAllowsScrollersToOverlapContent:(BOOL)flag
{
    if (allowsScrollersToOverlapContent == flag)
        return;
        
    allowsScrollersToOverlapContent = flag;
    
    [[self contentView] setFrame:[self contentViewFrame]];
    [[self documentView] setNeedsLayout:YES];
    [[self documentView] layout];
}

- (void)setAlwaysHideHorizontalScroller:(BOOL)shouldBeHidden
{
    if (alwaysHideHorizontalScroller == shouldBeHidden)
        return;

    alwaysHideHorizontalScroller = shouldBeHidden;
    [self updateScrollers];
}

- (void)setAlwaysHideVerticalScroller:(BOOL)shouldBeHidden
{
    if (alwaysHideVerticalScroller == shouldBeHidden)
        return;
        
    alwaysHideVerticalScroller = shouldBeHidden;
    [self updateScrollers];
}

- (BOOL)horizontalScrollingAllowed
{
    return horizontalScrollingAllowedButScrollerHidden || [self hasHorizontalScroller];
}

- (BOOL)verticalScrollingAllowed
{
    return verticalScrollingAllowedButScrollerHidden || [self hasVerticalScroller];
}

@end

@implementation WebDynamicScrollBarsView (WebInternal)

- (NSRect)contentViewFrame
{
    NSRect frame = [[self contentView] frame];
    
    if ([self hasHorizontalScroller])
        frame.size.height = (allowsScrollersToOverlapContent ? NSMaxY([[self horizontalScroller] frame]) : NSMinY([[self horizontalScroller] frame]));
    if ([self hasVerticalScroller])
        frame.size.width = (allowsScrollersToOverlapContent ? NSMaxX([[self verticalScroller] frame]) : NSMinX([[self verticalScroller] frame]));
    return frame;
}

- (void)tile
{
    [super tile];

    // [super tile] sets the contentView size so that it does not overlap with the scrollers,
    // we want to re-set the contentView to overlap scrollers before displaying.
    if (allowsScrollersToOverlapContent)
        [[self contentView] setFrame:[self contentViewFrame]];
}

- (void)setSuppressLayout:(BOOL)flag;
{
    suppressLayout = flag;
}

- (void)setScrollBarsSuppressed:(BOOL)suppressed repaintOnUnsuppress:(BOOL)repaint
{
    suppressScrollers = suppressed;

    // This code was originally changes for a Leopard performance imporvement. We decided to 
    // ifdef it to fix correctness issues on Tiger documented in <rdar://problem/5441823>.
#ifndef BUILDING_ON_TIGER
    if (suppressed) {
        [[self verticalScroller] setNeedsDisplay:NO];
        [[self horizontalScroller] setNeedsDisplay:NO];
    }
        
    if (!suppressed && repaint)
        [super reflectScrolledClipView:[self contentView]];
#else
    if (suppressed || repaint) { 
        [[self verticalScroller] setNeedsDisplay: !suppressed]; 
        [[self horizontalScroller] setNeedsDisplay: !suppressed]; 
    }
#endif
}

static const unsigned cMaxUpdateScrollbarsPass = 2;

- (void)updateScrollers
{
    NSView *documentView = [self documentView];

    // If we came in here with the view already needing a layout, then go ahead and do that
    // first.  (This will be the common case, e.g., when the page changes due to window resizing for example).
    // This layout will not re-enter updateScrollers and does not count towards our max layout pass total.
    if (!suppressLayout && !suppressScrollers && [documentView isKindOfClass:[WebHTMLView class]]) {
        WebHTMLView* htmlView = (WebHTMLView*)documentView;
        if ([htmlView _needsLayout]) {
            inUpdateScrollers = YES;
            [(id <WebDocumentView>)documentView layout];
            inUpdateScrollers = NO;
        }
    }

    BOOL hasHorizontalScroller = [self hasHorizontalScroller];
    BOOL hasVerticalScroller = [self hasVerticalScroller];
    
    BOOL newHasHorizontalScroller = hasHorizontalScroller;
    BOOL newHasVerticalScroller = hasVerticalScroller;
    
    if (!documentView) {
        newHasHorizontalScroller = NO;
        newHasVerticalScroller = NO;
    }

    if (hScroll != ScrollbarAuto)
        newHasHorizontalScroller = (hScroll == ScrollbarAlwaysOn);
    if (vScroll != ScrollbarAuto)
        newHasVerticalScroller = (vScroll == ScrollbarAlwaysOn);
    
    if (!documentView || suppressLayout || suppressScrollers || (hScroll != ScrollbarAuto && vScroll != ScrollbarAuto)) {
        horizontalScrollingAllowedButScrollerHidden = newHasHorizontalScroller && alwaysHideHorizontalScroller;
        if (horizontalScrollingAllowedButScrollerHidden)
            newHasHorizontalScroller = NO;
        
        verticalScrollingAllowedButScrollerHidden = newHasVerticalScroller && alwaysHideVerticalScroller;
        if (verticalScrollingAllowedButScrollerHidden)
            newHasVerticalScroller = NO;
        
        inUpdateScrollers = YES;
        if (hasHorizontalScroller != newHasHorizontalScroller)
            [self setHasHorizontalScroller:newHasHorizontalScroller];
        if (hasVerticalScroller != newHasVerticalScroller)
            [self setHasVerticalScroller:newHasVerticalScroller];
        if (suppressScrollers) {
            [[self verticalScroller] setNeedsDisplay:NO];
            [[self horizontalScroller] setNeedsDisplay:NO];
        }
        inUpdateScrollers = NO;
        return;
    }

    BOOL needsLayout = NO;

    NSSize documentSize = [documentView frame].size;
    NSSize visibleSize = [self documentVisibleRect].size;
    NSSize frameSize = [self frame].size;

    if (hScroll == ScrollbarAuto) {
        newHasHorizontalScroller = documentSize.width > visibleSize.width;
        if (newHasHorizontalScroller && !inUpdateScrollersLayoutPass && documentSize.height <= frameSize.height && documentSize.width <= frameSize.width)
            newHasHorizontalScroller = NO;
    }
    
    if (vScroll == ScrollbarAuto) {
        newHasVerticalScroller = documentSize.height > visibleSize.height;
        if (newHasVerticalScroller && !inUpdateScrollersLayoutPass && documentSize.height <= frameSize.height && documentSize.width <= frameSize.width)
            newHasVerticalScroller = NO;
    }

    // Unless in ScrollbarsAlwaysOn mode, if we ever turn one scrollbar off, always turn the other one off too.
    // Never ever try to both gain/lose a scrollbar in the same pass.
    if (!newHasHorizontalScroller && hasHorizontalScroller && vScroll != ScrollbarAlwaysOn)
        newHasVerticalScroller = NO;
    if (!newHasVerticalScroller && hasVerticalScroller && hScroll != ScrollbarAlwaysOn)
        newHasHorizontalScroller = NO;

    horizontalScrollingAllowedButScrollerHidden = newHasHorizontalScroller && alwaysHideHorizontalScroller;
    if (horizontalScrollingAllowedButScrollerHidden)
        newHasHorizontalScroller = NO;
    
    verticalScrollingAllowedButScrollerHidden = newHasVerticalScroller && alwaysHideVerticalScroller;
    if (verticalScrollingAllowedButScrollerHidden)
        newHasVerticalScroller = NO;
        
    if (hasHorizontalScroller != newHasHorizontalScroller) {
        inUpdateScrollers = YES;
        [self setHasHorizontalScroller:newHasHorizontalScroller];
        inUpdateScrollers = NO;
        needsLayout = YES;
    }

    if (hasVerticalScroller != newHasVerticalScroller) {
        inUpdateScrollers = YES;
        [self setHasVerticalScroller:newHasVerticalScroller];
        inUpdateScrollers = NO;
        needsLayout = YES;
    }

    if (needsLayout && inUpdateScrollersLayoutPass < cMaxUpdateScrollbarsPass && 
        [documentView conformsToProtocol:@protocol(WebDocumentView)]) {
        inUpdateScrollersLayoutPass++;
        [(id <WebDocumentView>)documentView setNeedsLayout:YES];
        [(id <WebDocumentView>)documentView layout];
        NSSize newDocumentSize = [documentView frame].size;
        if (NSEqualSizes(documentSize, newDocumentSize)) {
            // The layout with the new scroll state had no impact on
            // the document's overall size, so updateScrollers didn't get called.
            // Recur manually.
            [self updateScrollers];
        }
        inUpdateScrollersLayoutPass--;
    }
}

// Make the horizontal and vertical scroll bars come and go as needed.
- (void)reflectScrolledClipView:(NSClipView *)clipView
{
    if (clipView == [self contentView]) {
        // Prevent appearance of trails because of overlapping views
        if (allowsScrollersToOverlapContent)
            [self setDrawsBackground:NO];
    
        // FIXME: This hack here prevents infinite recursion that takes place when we
        // gyrate between having a vertical scroller and not having one. A reproducible
        // case is clicking on the "the Policy Routing text" link at
        // http://www.linuxpowered.com/archive/howto/Net-HOWTO-8.html.
        // The underlying cause is some problem in the NSText machinery, but I was not
        // able to pin it down.
        NSGraphicsContext *currentContext = [NSGraphicsContext currentContext];
        if (!inUpdateScrollers && (!currentContext || [currentContext isDrawingToScreen]))
            [self updateScrollers];
    }

    // This code was originally changed for a Leopard performance imporvement. We decided to 
    // ifdef it to fix correctness issues on Tiger documented in <rdar://problem/5441823>.
#ifndef BUILDING_ON_TIGER
    // Update the scrollers if they're not being suppressed.
    if (!suppressScrollers)
        [super reflectScrolledClipView:clipView];
#else
    [super reflectScrolledClipView:clipView]; 
  
    // Validate the scrollers if they're being suppressed. 
    if (suppressScrollers) { 
        [[self verticalScroller] setNeedsDisplay: NO]; 
        [[self horizontalScroller] setNeedsDisplay: NO]; 
    }
#endif

#if USE(ACCELERATED_COMPOSITING) && defined(BUILDING_ON_LEOPARD)
    NSView *documentView = [self documentView];
    if ([documentView isKindOfClass:[WebHTMLView class]]) {
        WebHTMLView *htmlView = (WebHTMLView *)documentView;
        if ([htmlView _isUsingAcceleratedCompositing])
            [htmlView _updateLayerHostingViewPosition];
    }
#endif
}

- (BOOL)allowsHorizontalScrolling
{
    return hScroll != ScrollbarAlwaysOff;
}

- (BOOL)allowsVerticalScrolling
{
    return vScroll != ScrollbarAlwaysOff;
}

- (void)scrollingModes:(WebCore::ScrollbarMode*)hMode vertical:(WebCore::ScrollbarMode*)vMode
{
    *hMode = static_cast<ScrollbarMode>(hScroll);
    *vMode = static_cast<ScrollbarMode>(vScroll);
}

- (ScrollbarMode)horizontalScrollingMode
{
    return static_cast<ScrollbarMode>(hScroll);
}

- (ScrollbarMode)verticalScrollingMode
{
    return static_cast<ScrollbarMode>(vScroll);
}

- (void)setHorizontalScrollingMode:(ScrollbarMode)horizontalMode andLock:(BOOL)lock
{
    [self setScrollingModes:horizontalMode vertical:[self verticalScrollingMode] andLock:lock];
}

- (void)setVerticalScrollingMode:(ScrollbarMode)verticalMode andLock:(BOOL)lock
{
    [self setScrollingModes:[self horizontalScrollingMode] vertical:verticalMode andLock:lock];
}

// Mail uses this method, so we cannot remove it. 
- (void)setVerticalScrollingMode:(ScrollbarMode)verticalMode 
{ 
    [self setScrollingModes:[self horizontalScrollingMode] vertical:verticalMode andLock:NO]; 
} 

- (void)setScrollingModes:(ScrollbarMode)horizontalMode vertical:(ScrollbarMode)verticalMode andLock:(BOOL)lock
{
    BOOL update = NO;
    if (verticalMode != vScroll && !vScrollModeLocked) {
        vScroll = verticalMode;
        update = YES;
    }

    if (horizontalMode != hScroll && !hScrollModeLocked) {
        hScroll = horizontalMode;
        update = YES;
    }

    if (lock)
        [self setScrollingModesLocked:YES];

    if (update)
        [self updateScrollers];
}

- (void)setHorizontalScrollingModeLocked:(BOOL)locked
{
    hScrollModeLocked = locked;
}

- (void)setVerticalScrollingModeLocked:(BOOL)locked
{
    vScrollModeLocked = locked;
}

- (void)setScrollingModesLocked:(BOOL)locked
{
    hScrollModeLocked = vScrollModeLocked = locked;
}

- (BOOL)horizontalScrollingModeLocked
{
    return hScrollModeLocked;
}

- (BOOL)verticalScrollingModeLocked
{
    return vScrollModeLocked;
}

- (BOOL)autoforwardsScrollWheelEvents
{
    return YES;
}

- (void)scrollWheel:(NSEvent *)event
{
    float deltaX;
    float deltaY;
    float wheelTicksX;
    float wheelTicksY;
    BOOL isContinuous;
    WKGetWheelEventDeltas(event, &deltaX, &deltaY, &wheelTicksX, &wheelTicksY, &isContinuous);

    BOOL isLatchingEvent = WKIsLatchingWheelEvent(event);

    if (fabsf(deltaY) > fabsf(deltaX)) {
        if (![self allowsVerticalScrolling]) {
            [[self nextResponder] scrollWheel:event];
            return;
        }

        if (isLatchingEvent && !verticallyPinnedByPreviousWheelEvent) {
            double verticalPosition = [[self verticalScroller] doubleValue];
            if ((deltaY >= 0.0 && verticalPosition == 0.0) || (deltaY <= 0.0 && verticalPosition == 1.0))
                return;
        }
    } else {
        if (![self allowsHorizontalScrolling]) {
            [[self nextResponder] scrollWheel:event];
            return;
        }

        if (isLatchingEvent && !horizontallyPinnedByPreviousWheelEvent) {
            double horizontalPosition = [[self horizontalScroller] doubleValue];
            if ((deltaX >= 0.0 && horizontalPosition == 0.0) || (deltaX <= 0.0 && horizontalPosition == 1.0))
                return;
        }
    }

    // Calling super can release the last reference. <rdar://problem/7400263>
    // Hold a reference so the code following the super call will not crash.
    [self retain];

    [super scrollWheel:event];

    if (!isLatchingEvent) {
        double verticalPosition = [[self verticalScroller] doubleValue];
        double horizontalPosition = [[self horizontalScroller] doubleValue];

        verticallyPinnedByPreviousWheelEvent = (verticalPosition == 0.0 || verticalPosition == 1.0);
        horizontallyPinnedByPreviousWheelEvent = (horizontalPosition == 0.0 || horizontalPosition == 1.0);
    }

    [self release];
}

- (BOOL)accessibilityIsIgnored 
{
    id docView = [self documentView];
    if ([docView isKindOfClass:[WebFrameView class]] && ![(WebFrameView *)docView allowsScrolling])
        return YES;
    
    return [super accessibilityIsIgnored];
}

@end
