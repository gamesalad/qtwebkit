/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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

#include "config.h"
#include "V8History.h"

#include "ExceptionCode.h"
#include "History.h"
#include "SerializedScriptValue.h"
#include "V8Binding.h"
#include "V8BindingState.h"
#include "V8DOMWindow.h"
#include "V8Proxy.h"

namespace WebCore {

v8::Handle<v8::Value> V8History::pushStateCallback(const v8::Arguments& args)
{
    RefPtr<SerializedScriptValue> historyState = SerializedScriptValue::create(args[0]);

    v8::TryCatch tryCatch;
    String title = toWebCoreStringWithNullOrUndefinedCheck(args[1]);
    if (tryCatch.HasCaught())
        return v8::Undefined();
    String url;
    if (args.Length() > 2) {
        url = toWebCoreStringWithNullOrUndefinedCheck(args[2]);
        if (tryCatch.HasCaught())
            return v8::Undefined();
    }

    ExceptionCode ec = 0;
    History* history = V8History::toNative(args.Holder());
    history->stateObjectAdded(historyState.release(), title, url, History::StateObjectPush, ec);
    return throwError(ec);
}

v8::Handle<v8::Value> V8History::replaceStateCallback(const v8::Arguments& args)
{
    RefPtr<SerializedScriptValue> historyState = SerializedScriptValue::create(args[0]);

    v8::TryCatch tryCatch;
    String title = toWebCoreStringWithNullOrUndefinedCheck(args[1]);
    if (tryCatch.HasCaught())
        return v8::Undefined();
    String url;
    if (args.Length() > 2) {
        url = toWebCoreStringWithNullOrUndefinedCheck(args[2]);
        if (tryCatch.HasCaught())
            return v8::Undefined();
    }

    ExceptionCode ec = 0;
    History* history = V8History::toNative(args.Holder());
    history->stateObjectAdded(historyState.release(), title, url, History::StateObjectReplace, ec);
    return throwError(ec);
}

bool V8History::indexedSecurityCheck(v8::Local<v8::Object> host, uint32_t index, v8::AccessType type, v8::Local<v8::Value>)
{
    // Only allow same origin access.
    History* history = V8History::toNative(host);
    return V8BindingSecurity::canAccessFrame(V8BindingState::Only(), history->frame(), false);
}

bool V8History::namedSecurityCheck(v8::Local<v8::Object> host, v8::Local<v8::Value> key, v8::AccessType type, v8::Local<v8::Value>)
{
    // Only allow same origin access.
    History* history = V8History::toNative(host);
    return V8BindingSecurity::canAccessFrame(V8BindingState::Only(), history->frame(), false);
}

v8::Handle<v8::Value> toV8(History* impl)
{
    if (!impl)
        return v8::Null();
    v8::Handle<v8::Object> wrapper = getDOMObjectMap().get(impl);
    if (wrapper.IsEmpty()) {
        wrapper = V8History::wrap(impl);
        if (!wrapper.IsEmpty())
            V8DOMWrapper::setHiddenWindowReference(impl->frame(), V8DOMWindow::historyIndex, wrapper);
    }
    return wrapper;
}

} // namespace WebCore
