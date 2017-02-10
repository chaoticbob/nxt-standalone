// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "RefCounted.h"

#include <cassert>
#define ASSERT assert

namespace backend {

    RefCounted::RefCounted() {
    }

    RefCounted::~RefCounted() {
    }

    void RefCounted::ReferenceInternal() {
        ASSERT(internalRefs != 0);
        // TODO(cwallez@chromium.org): what to do on overflow?
        internalRefs ++;
    }

    void RefCounted::ReleaseInternal() {
        ASSERT(internalRefs != 0);
        internalRefs --;
        if (internalRefs == 0) {
            ASSERT(externalRefs == 0);
            // TODO(cwallez@chromium.org): would this work with custom allocators?
            delete this;
        }
    }

    uint32_t RefCounted::GetExternalRefs() const {
        return externalRefs;
    }

    uint32_t RefCounted::GetInternalRefs() const {
        return internalRefs;
    }

    void RefCounted::Reference() {
        ASSERT(externalRefs != 0);
        // TODO(cwallez@chromium.org): what to do on overflow?
        externalRefs ++;
    }

    void RefCounted::Release() {
        ASSERT(externalRefs != 0);
        externalRefs --;
        if (externalRefs == 0) {
            ReleaseInternal();
        }
    }

}
