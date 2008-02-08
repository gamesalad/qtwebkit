/*
 * Copyright (C) 2007, 2008 Apple Inc. All rights reserved.
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
#include "JSNodeFilter.h"

#include "JSNodeFilterCondition.h"
#include "NodeFilter.h"
#include "kjs_binding.h"
#include "kjs_dom.h"

using namespace KJS;

namespace WebCore {

void JSNodeFilter::mark()
{
    impl()->mark();
    DOMObject::mark();
}

JSValue* JSNodeFilter::acceptNode(ExecState* exec, const List& args)
{
    JSValue* exception = 0;
    short result = impl()->acceptNode(toNode(args[0]), exception);
    if (exception)
        exec->setException(exception);
    return jsNumber(result);
}

NodeFilter* toNodeFilter(KJS::JSValue* val)
{
    if (!val || !val->isObject())
        return 0;

    if (val->isObject(&JSNodeFilter::info))
        return static_cast<JSNodeFilter*>(val)->impl();

    KJS::JSObject* o = static_cast<KJS::JSObject*>(val);
    if (o->implementsCall())
        return new NodeFilter(new JSNodeFilterCondition(o));

    return 0;
}

} // namespace WebCore
