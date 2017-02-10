// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BACKEND_COMMON_INPUTSTATE_H_
#define BACKEND_COMMON_INPUTSTATE_H_

#include "Forward.h"
#include "RefCounted.h"

#include "nxt/nxtcpp.h"

#include <array>
#include <bitset>

namespace backend {

    size_t IndexFormatSize(nxt::IndexFormat format);
    uint32_t VertexFormatNumComponents(nxt::VertexFormat format);
    size_t VertexFormatSize(nxt::VertexFormat format);

    class InputStateBase : public RefCounted {
        public:
            InputStateBase(InputStateBuilder* builder);

            struct AttributeInfo {
                uint32_t bindingSlot;
                nxt::VertexFormat format;
                uint32_t offset;
            };

            struct InputInfo {
                uint32_t stride;
                nxt::InputStepMode stepMode;
            };

            const std::bitset<kMaxVertexAttributes>& GetAttributesSetMask() const;
            const AttributeInfo& GetAttribute(uint32_t location) const;
            const std::bitset<kMaxVertexInputs>& GetInputsSetMask() const;
            const InputInfo& GetInput(uint32_t slot) const;

        private:
            std::bitset<kMaxVertexAttributes> attributesSetMask;
            std::array<AttributeInfo, kMaxVertexAttributes> attributeInfos;
            std::bitset<kMaxVertexInputs> inputsSetMask;
            std::array<InputInfo, kMaxVertexInputs> inputInfos;
    };

    class InputStateBuilder : public RefCounted {
        public:
            InputStateBuilder(DeviceBase* device);

            bool WasConsumed() const;

            // NXT API
            InputStateBase* GetResult();
            void SetAttribute(uint32_t shaderLocation, uint32_t bindingSlot,
                    nxt::VertexFormat format, uint32_t offset);
            void SetInput(uint32_t bindingSlot, uint32_t stride,
                    nxt::InputStepMode stepMode);

        private:
            friend class InputStateBase;

            DeviceBase* device;
            std::bitset<kMaxVertexAttributes> attributesSetMask;
            std::array<InputStateBase::AttributeInfo, kMaxVertexAttributes> attributeInfos;
            std::bitset<kMaxVertexInputs> inputsSetMask;
            std::array<InputStateBase::InputInfo, kMaxVertexInputs> inputInfos;
            bool consumed = false;
    };

}

#endif // BACKEND_COMMON_INPUTSTATE_H_
