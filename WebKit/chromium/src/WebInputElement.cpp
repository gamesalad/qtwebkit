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
#include "WebInputElement.h"

#include "HTMLInputElement.h"
#include "HTMLNames.h"
#include "WebString.h"
#include <wtf/PassRefPtr.h>

using namespace WebCore;

namespace WebKit {

bool WebInputElement::autoComplete() const
{
    return constUnwrap<HTMLInputElement>()->autoComplete();
}

bool WebInputElement::isEnabledFormControl() const
{
    return constUnwrap<HTMLInputElement>()->isEnabledFormControl();
}

WebInputElement::InputType WebInputElement::inputType() const
{
    return static_cast<InputType>(constUnwrap<HTMLInputElement>()->inputType());
}

WebString WebInputElement::formControlType() const
{
    return constUnwrap<HTMLInputElement>()->formControlType();
}

bool WebInputElement::isActivatedSubmit() const
{
    return constUnwrap<HTMLInputElement>()->isActivatedSubmit();
}

void WebInputElement::setActivatedSubmit(bool activated)
{
    unwrap<HTMLInputElement>()->setActivatedSubmit(activated);
}

void WebInputElement::setValue(const WebString& value)
{
    unwrap<HTMLInputElement>()->setValue(value);
}

WebString WebInputElement::value() const
{
    return constUnwrap<HTMLInputElement>()->value();
}

void WebInputElement::setAutofilled(bool autoFilled)
{
    unwrap<HTMLInputElement>()->setAutofilled(autoFilled);
}

void WebInputElement::dispatchFormControlChangeEvent()
{
    unwrap<HTMLInputElement>()->dispatchFormControlChangeEvent();
}

void WebInputElement::setSelectionRange(int start, int end)
{
    unwrap<HTMLInputElement>()->setSelectionRange(start, end);
}
    
WebString WebInputElement::name() const
{
    return constUnwrap<HTMLInputElement>()->name();
}
    
WebString WebInputElement::nameForAutofill() const
{
    String name = constUnwrap<HTMLInputElement>()->name();
    String trimmedName = name.stripWhiteSpace();
    if (!trimmedName.isEmpty())
        return trimmedName;
    name = constUnwrap<HTMLInputElement>()->getAttribute(HTMLNames::idAttr);
    trimmedName = name.stripWhiteSpace();
    if (!trimmedName.isEmpty())
        return trimmedName;
    return String();
}

WebInputElement::WebInputElement(const PassRefPtr<HTMLInputElement>& elem)
    : WebElement(elem)
{
}

WebInputElement& WebInputElement::operator=(const PassRefPtr<HTMLInputElement>& elem)
{
    m_private = elem;
    return *this;
}

WebInputElement::operator PassRefPtr<HTMLInputElement>() const
{
    return static_cast<HTMLInputElement*>(m_private.get());
}

} // namespace WebKit
