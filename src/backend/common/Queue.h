// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BACKEND_COMMON_QUEUE_H_
#define BACKEND_COMMON_QUEUE_H_

#include "Forward.h"
#include "RefCounted.h"

#include "nxt/nxtcpp.h"

namespace backend {

    class QueueBase : public RefCounted {
    };

    class QueueBuilder : public RefCounted {
        public:
            QueueBuilder(DeviceBase* device);

            bool WasConsumed() const;

            // NXT API
            QueueBase* GetResult();

        private:
            DeviceBase* device;
            bool consumed = false;
    };

}

#endif // BACKEND_COMMON_QUEUE_H_
