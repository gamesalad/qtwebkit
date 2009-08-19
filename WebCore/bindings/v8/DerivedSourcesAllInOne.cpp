/*
 * Copyright (c) 2009, Google Inc. All rights reserved.
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

// This source file coalesces the HTML elements into a single object file to
// reduce bloat and allow us to link release builds on 32-bit Windows.

#include "bindings/V8AbstractWorker.cpp"
#include "bindings/V8Attr.cpp"
#include "bindings/V8BarInfo.cpp"
#include "bindings/V8CanvasGradient.cpp"
#include "bindings/V8CanvasPattern.cpp"
#include "bindings/V8CanvasPixelArray.cpp"
#include "bindings/V8CanvasRenderingContext2D.cpp"
#include "bindings/V8CDATASection.cpp"
#include "bindings/V8CharacterData.cpp"
#include "bindings/V8ClientRect.cpp"
#include "bindings/V8ClientRectList.cpp"
#include "bindings/V8Clipboard.cpp"
#include "bindings/V8Comment.cpp"
#include "bindings/V8Console.cpp"
#include "bindings/V8Counter.cpp"
#include "bindings/V8CSSCharsetRule.cpp"
#include "bindings/V8CSSFontFaceRule.cpp"
#include "bindings/V8CSSImportRule.cpp"
#include "bindings/V8CSSMediaRule.cpp"
#include "bindings/V8CSSPageRule.cpp"
#include "bindings/V8CSSPrimitiveValue.cpp"
#include "bindings/V8CSSRule.cpp"
#include "bindings/V8CSSRuleList.cpp"
#include "bindings/V8CSSStyleDeclaration.cpp"
#include "bindings/V8CSSStyleRule.cpp"
#include "bindings/V8CSSStyleSheet.cpp"
#include "bindings/V8CSSValue.cpp"
#include "bindings/V8CSSValueList.cpp"
#include "bindings/V8CSSVariablesDeclaration.cpp"
#include "bindings/V8CSSVariablesRule.cpp"
#include "bindings/V8Database.cpp"
#include "bindings/V8DataGridColumn.cpp"
#include "bindings/V8DataGridColumnList.cpp"
#include "bindings/V8DedicatedWorkerContext.cpp"
#include "bindings/V8Document.cpp"
#include "bindings/V8DocumentFragment.cpp"
#include "bindings/V8DocumentType.cpp"
#if ENABLE(OFFLINE_WEB_APPLICATIONS)
#include "bindings/V8DOMApplicationCache.cpp"
#endif
#include "bindings/V8DOMCoreException.cpp"
#include "bindings/V8DOMImplementation.cpp"
#include "bindings/V8DOMParser.cpp"
#include "bindings/V8DOMSelection.cpp"
#include "bindings/V8DOMWindow.cpp"
#include "bindings/V8Element.cpp"
#include "bindings/V8Entity.cpp"
#include "bindings/V8EntityReference.cpp"
#include "bindings/V8ErrorEvent.cpp"
#include "bindings/V8Event.cpp"
#include "bindings/V8EventException.cpp"
#include "bindings/V8File.cpp"
#include "bindings/V8FileList.cpp"
#include "bindings/V8History.cpp"
#include "bindings/V8HTMLAllCollection.cpp"
#include "bindings/V8HTMLAnchorElement.cpp"
#include "bindings/V8HTMLAppletElement.cpp"
#include "bindings/V8HTMLAreaElement.cpp"
#include "bindings/V8HTMLAudioElement.cpp"
#include "bindings/V8HTMLBaseElement.cpp"
#include "bindings/V8HTMLBaseFontElement.cpp"
#include "bindings/V8HTMLBlockquoteElement.cpp"
#include "bindings/V8HTMLBodyElement.cpp"
#include "bindings/V8HTMLBRElement.cpp"
#include "bindings/V8HTMLButtonElement.cpp"
#include "bindings/V8HTMLCanvasElement.cpp"
#include "bindings/V8HTMLCollection.cpp"
#include "bindings/V8HTMLDataGridCellElement.cpp"
#include "bindings/V8HTMLDataGridColElement.cpp"
#include "bindings/V8HTMLDataGridElement.cpp"
#include "bindings/V8HTMLDataGridRowElement.cpp"
#include "bindings/V8HTMLDirectoryElement.cpp"
#include "bindings/V8HTMLDivElement.cpp"
#include "bindings/V8HTMLDListElement.cpp"
#include "bindings/V8HTMLDocument.cpp"
#include "bindings/V8HTMLElement.cpp"
#include "bindings/V8HTMLEmbedElement.cpp"
#include "bindings/V8HTMLFieldSetElement.cpp"
#include "bindings/V8HTMLFontElement.cpp"
#include "bindings/V8HTMLFormElement.cpp"
#include "bindings/V8HTMLFrameElement.cpp"
#include "bindings/V8HTMLFrameSetElement.cpp"
#include "bindings/V8HTMLHeadElement.cpp"
#include "bindings/V8HTMLHeadingElement.cpp"
#include "bindings/V8HTMLHRElement.cpp"
#include "bindings/V8HTMLHtmlElement.cpp"
#include "bindings/V8HTMLIFrameElement.cpp"
#include "bindings/V8HTMLImageElement.cpp"
#include "bindings/V8HTMLInputElement.cpp"
#include "bindings/V8HTMLIsIndexElement.cpp"
#include "bindings/V8HTMLLabelElement.cpp"
#include "bindings/V8HTMLLegendElement.cpp"
#include "bindings/V8HTMLLIElement.cpp"
#include "bindings/V8HTMLLinkElement.cpp"
#include "bindings/V8HTMLMapElement.cpp"
#include "bindings/V8HTMLMarqueeElement.cpp"
#include "bindings/V8HTMLMediaElement.cpp"
#include "bindings/V8HTMLMenuElement.cpp"
#include "bindings/V8HTMLMetaElement.cpp"
#include "bindings/V8HTMLModElement.cpp"
#include "bindings/V8HTMLObjectElement.cpp"
#include "bindings/V8HTMLOListElement.cpp"
#include "bindings/V8HTMLOptGroupElement.cpp"
#include "bindings/V8HTMLOptionElement.cpp"
#include "bindings/V8HTMLOptionsCollection.cpp"
#include "bindings/V8HTMLParagraphElement.cpp"
#include "bindings/V8HTMLParamElement.cpp"
#include "bindings/V8HTMLPreElement.cpp"
#include "bindings/V8HTMLQuoteElement.cpp"
#include "bindings/V8HTMLScriptElement.cpp"
#include "bindings/V8HTMLSelectElement.cpp"
#include "bindings/V8HTMLSourceElement.cpp"
#include "bindings/V8HTMLStyleElement.cpp"
#include "bindings/V8HTMLTableCaptionElement.cpp"
#include "bindings/V8HTMLTableCellElement.cpp"
#include "bindings/V8HTMLTableColElement.cpp"
#include "bindings/V8HTMLTableElement.cpp"
#include "bindings/V8HTMLTableRowElement.cpp"
#include "bindings/V8HTMLTableSectionElement.cpp"
#include "bindings/V8HTMLTextAreaElement.cpp"
#include "bindings/V8HTMLTitleElement.cpp"
#include "bindings/V8HTMLUListElement.cpp"
#include "bindings/V8HTMLVideoElement.cpp"
#include "bindings/V8ImageData.cpp"
#include "bindings/V8InspectorBackend.cpp"
#include "bindings/V8KeyboardEvent.cpp"
#include "bindings/V8Location.cpp"
#include "bindings/V8Media.cpp"
#include "bindings/V8MediaError.cpp"
#include "bindings/V8MediaList.cpp"
#include "bindings/V8MessageChannel.cpp"
#include "bindings/V8MessageEvent.cpp"
#include "bindings/V8MessagePort.cpp"
#include "bindings/V8MimeType.cpp"
#include "bindings/V8MimeTypeArray.cpp"
#include "bindings/V8MouseEvent.cpp"
#include "bindings/V8MutationEvent.cpp"
#include "bindings/V8NamedNodeMap.cpp"
#include "bindings/V8Navigator.cpp"
#include "bindings/V8Node.cpp"
#include "bindings/V8NodeFilter.cpp"
#include "bindings/V8NodeIterator.cpp"
#include "bindings/V8NodeList.cpp"
#include "bindings/V8Notation.cpp"
#include "bindings/V8Notification.cpp"
#include "bindings/V8NotificationCenter.cpp"
#include "bindings/V8OverflowEvent.cpp"
#include "bindings/V8Plugin.cpp"
#include "bindings/V8PluginArray.cpp"
#include "bindings/V8ProcessingInstruction.cpp"
#include "bindings/V8ProgressEvent.cpp"
#include "bindings/V8Range.cpp"
#include "bindings/V8RangeException.cpp"
#include "bindings/V8Rect.cpp"
#include "bindings/V8RGBColor.cpp"
#include "bindings/V8Screen.cpp"
#include "bindings/V8SharedWorker.cpp"
#include "bindings/V8SQLError.cpp"
#include "bindings/V8SQLResultSet.cpp"
#include "bindings/V8SQLResultSetRowList.cpp"
#include "bindings/V8SQLTransaction.cpp"
#if ENABLE(DOM_STORAGE)
#include "bindings/V8Storage.cpp"
#include "bindings/V8StorageEvent.cpp"
#endif
#include "bindings/V8StyleSheet.cpp"
#include "bindings/V8StyleSheetList.cpp"
#include "bindings/V8SVGAElement.cpp"
#include "bindings/V8SVGAltGlyphElement.cpp"
#include "bindings/V8SVGAngle.cpp"
#include "bindings/V8SVGAnimateColorElement.cpp"
#include "bindings/V8SVGAnimatedAngle.cpp"
#include "bindings/V8SVGAnimatedBoolean.cpp"
#include "bindings/V8SVGAnimatedEnumeration.cpp"
#include "bindings/V8SVGAnimatedInteger.cpp"
#include "bindings/V8SVGAnimatedLength.cpp"
#include "bindings/V8SVGAnimatedLengthList.cpp"
#include "bindings/V8SVGAnimatedNumber.cpp"
#include "bindings/V8SVGAnimatedNumberList.cpp"
#include "bindings/V8SVGAnimatedPoints.cpp"
#include "bindings/V8SVGAnimatedPreserveAspectRatio.cpp"
#include "bindings/V8SVGAnimatedRect.cpp"
#include "bindings/V8SVGAnimatedString.cpp"
#include "bindings/V8SVGAnimatedTransformList.cpp"
#include "bindings/V8SVGAnimateElement.cpp"
#include "bindings/V8SVGAnimateTransformElement.cpp"
#include "bindings/V8SVGAnimationElement.cpp"
#include "bindings/V8SVGCircleElement.cpp"
#include "bindings/V8SVGClipPathElement.cpp"
#include "bindings/V8SVGColor.cpp"
#include "bindings/V8SVGCursorElement.cpp"
#include "bindings/V8SVGDefinitionSrcElement.cpp"
#include "bindings/V8SVGDefsElement.cpp"
#include "bindings/V8SVGDescElement.cpp"
#include "bindings/V8SVGDocument.cpp"
#include "bindings/V8SVGElement.cpp"
#include "bindings/V8SVGElementInstance.cpp"
#include "bindings/V8SVGElementInstanceList.cpp"
#include "bindings/V8SVGEllipseElement.cpp"
#include "bindings/V8SVGException.cpp"
#include "bindings/V8SVGFontElement.cpp"
#include "bindings/V8SVGFontFaceElement.cpp"
#include "bindings/V8SVGFontFaceFormatElement.cpp"
#include "bindings/V8SVGFontFaceNameElement.cpp"
#include "bindings/V8SVGFontFaceSrcElement.cpp"
#include "bindings/V8SVGFontFaceUriElement.cpp"
#include "bindings/V8SVGForeignObjectElement.cpp"
#include "bindings/V8SVGGElement.cpp"
#include "bindings/V8SVGGlyphElement.cpp"
#include "bindings/V8SVGGradientElement.cpp"
#include "bindings/V8SVGImageElement.cpp"
#include "bindings/V8SVGLength.cpp"
#include "bindings/V8SVGLengthList.cpp"
#include "bindings/V8SVGLinearGradientElement.cpp"
#include "bindings/V8SVGLineElement.cpp"
#include "bindings/V8SVGMarkerElement.cpp"
#include "bindings/V8SVGMaskElement.cpp"
#include "bindings/V8SVGMatrix.cpp"
#include "bindings/V8SVGMetadataElement.cpp"
#include "bindings/V8SVGMissingGlyphElement.cpp"
#include "bindings/V8SVGNumber.cpp"
#include "bindings/V8SVGNumberList.cpp"
#include "bindings/V8SVGPaint.cpp"
#include "bindings/V8SVGPathElement.cpp"
#include "bindings/V8SVGPathSeg.cpp"
#include "bindings/V8SVGPathSegArcAbs.cpp"
#include "bindings/V8SVGPathSegArcRel.cpp"
#include "bindings/V8SVGPathSegClosePath.cpp"
#include "bindings/V8SVGPathSegCurvetoCubicAbs.cpp"
#include "bindings/V8SVGPathSegCurvetoCubicRel.cpp"
#include "bindings/V8SVGPathSegCurvetoCubicSmoothAbs.cpp"
#include "bindings/V8SVGPathSegCurvetoCubicSmoothRel.cpp"
#include "bindings/V8SVGPathSegCurvetoQuadraticAbs.cpp"
#include "bindings/V8SVGPathSegCurvetoQuadraticRel.cpp"
#include "bindings/V8SVGPathSegCurvetoQuadraticSmoothAbs.cpp"
#include "bindings/V8SVGPathSegCurvetoQuadraticSmoothRel.cpp"
#include "bindings/V8SVGPathSegLinetoAbs.cpp"
#include "bindings/V8SVGPathSegLinetoHorizontalAbs.cpp"
#include "bindings/V8SVGPathSegLinetoHorizontalRel.cpp"
#include "bindings/V8SVGPathSegLinetoRel.cpp"
#include "bindings/V8SVGPathSegLinetoVerticalAbs.cpp"
#include "bindings/V8SVGPathSegLinetoVerticalRel.cpp"
#include "bindings/V8SVGPathSegList.cpp"
#include "bindings/V8SVGPathSegMovetoAbs.cpp"
#include "bindings/V8SVGPathSegMovetoRel.cpp"
#include "bindings/V8SVGPatternElement.cpp"
#include "bindings/V8SVGPoint.cpp"
#include "bindings/V8SVGPointList.cpp"
#include "bindings/V8SVGPolygonElement.cpp"
#include "bindings/V8SVGPolylineElement.cpp"
#include "bindings/V8SVGPreserveAspectRatio.cpp"
#include "bindings/V8SVGRadialGradientElement.cpp"
#include "bindings/V8SVGRect.cpp"
#include "bindings/V8SVGRectElement.cpp"
#include "bindings/V8SVGRenderingIntent.cpp"
#include "bindings/V8SVGScriptElement.cpp"
#include "bindings/V8SVGSetElement.cpp"
#include "bindings/V8SVGStopElement.cpp"
#include "bindings/V8SVGStringList.cpp"
#include "bindings/V8SVGStyleElement.cpp"
#include "bindings/V8SVGSVGElement.cpp"
#include "bindings/V8SVGSwitchElement.cpp"
#include "bindings/V8SVGSymbolElement.cpp"
#include "bindings/V8SVGTextContentElement.cpp"
#include "bindings/V8SVGTextElement.cpp"
#include "bindings/V8SVGTextPathElement.cpp"
#include "bindings/V8SVGTextPositioningElement.cpp"
#include "bindings/V8SVGTitleElement.cpp"
#include "bindings/V8SVGTransform.cpp"
#include "bindings/V8SVGTransformList.cpp"
#include "bindings/V8SVGTRefElement.cpp"
#include "bindings/V8SVGTSpanElement.cpp"
#include "bindings/V8SVGUnitTypes.cpp"
#include "bindings/V8SVGURIReference.cpp"
#include "bindings/V8SVGUseElement.cpp"
#include "bindings/V8SVGViewElement.cpp"
#include "bindings/V8SVGZoomEvent.cpp"
#include "bindings/V8Text.cpp"
#include "bindings/V8TextEvent.cpp"
#include "bindings/V8TextMetrics.cpp"
#include "bindings/V8TimeRanges.cpp"
#include "bindings/V8TreeWalker.cpp"
#include "bindings/V8UIEvent.cpp"
#include "bindings/V8ValidityState.cpp"
#include "bindings/V8WebKitAnimationEvent.cpp"
#include "bindings/V8WebKitCSSKeyframeRule.cpp"
#include "bindings/V8WebKitCSSKeyframesRule.cpp"
#include "bindings/V8WebKitCSSMatrix.cpp"
#include "bindings/V8WebKitCSSTransformValue.cpp"
#include "bindings/V8WebKitPoint.cpp"
#include "bindings/V8WebKitTransitionEvent.cpp"
#include "bindings/V8WheelEvent.cpp"
#include "bindings/V8Worker.cpp"
#include "bindings/V8WorkerContext.cpp"
#include "bindings/V8WorkerLocation.cpp"
#include "bindings/V8WorkerNavigator.cpp"
#include "bindings/V8XMLHttpRequest.cpp"
#include "bindings/V8XMLHttpRequestException.cpp"
#include "bindings/V8XMLHttpRequestProgressEvent.cpp"
#include "bindings/V8XMLHttpRequestUpload.cpp"
#include "bindings/V8XMLSerializer.cpp"
#include "bindings/V8XPathEvaluator.cpp"
#include "bindings/V8XPathException.cpp"
#include "bindings/V8XPathExpression.cpp"
#include "bindings/V8XPathNSResolver.cpp"
#include "bindings/V8XPathResult.cpp"
#include "bindings/V8XSLTProcessor.cpp"
