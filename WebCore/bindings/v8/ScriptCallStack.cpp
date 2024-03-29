/*
 * Copyright (C) 2008, 2009 Google Inc. All rights reserved.
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
#include "ScriptCallStack.h"

#include "ScriptController.h"

#include <v8.h>

#include "V8Binding.h"
#include "V8Proxy.h"

namespace WebCore {

ScriptCallStack* ScriptCallStack::create(const v8::Arguments& arguments, unsigned skipArgumentCount) {
    String sourceName;
    int sourceLineNumber;
    if (!callLocation(&sourceName, &sourceLineNumber))
      return 0;
    return new ScriptCallStack(arguments, skipArgumentCount, sourceName, sourceLineNumber);
}

bool ScriptCallStack::callLocation(String* sourceName, int* sourceLineNumber)
{
    if (!V8Proxy::sourceName(*sourceName) || !V8Proxy::sourceLineNumber(*sourceLineNumber))
        return false;
    *sourceLineNumber += 1;
    return true;
}

ScriptCallStack::ScriptCallStack(const v8::Arguments& arguments, unsigned skipArgumentCount, String sourceName, int sourceLineNumber)
    : m_lastCaller(String(), sourceName, sourceLineNumber, arguments, skipArgumentCount)
    , m_scriptState(ScriptState::current())
{
}

ScriptCallStack::~ScriptCallStack()
{
}

const ScriptCallFrame& ScriptCallStack::at(unsigned index) const
{
    // Currently, only one ScriptCallFrame is supported. When we can get
    // a full stack trace from V8, we can do this right.
    ASSERT(index == 0);
    return m_lastCaller;
}

} // namespace WebCore
