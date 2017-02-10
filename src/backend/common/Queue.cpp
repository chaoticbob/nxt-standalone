// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "Queue.h"

#include "Device.h"

namespace backend {

    // QueueBuilder

    QueueBuilder::QueueBuilder(DeviceBase* device) : device(device) {
    }

    bool QueueBuilder::WasConsumed() const {
        return consumed;
    }

    QueueBase* QueueBuilder::GetResult() {
        consumed = true;
        return device->CreateQueue(this);
    }

}
