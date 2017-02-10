// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "InputState.h"

#include "Device.h"

namespace backend {

    // InputState helpers

    size_t IndexFormatSize(nxt::IndexFormat format) {
        switch (format) {
            case nxt::IndexFormat::Uint16:
                return sizeof(uint16_t);
            case nxt::IndexFormat::Uint32:
                return sizeof(uint32_t);
        }
    }

    uint32_t VertexFormatNumComponents(nxt::VertexFormat format) {
        switch (format) {
            case nxt::VertexFormat::FloatR32G32B32A32:
                return 4;
            case nxt::VertexFormat::FloatR32G32B32:
                return 3;
            case nxt::VertexFormat::FloatR32G32:
                return 2;
        }
    }

    size_t VertexFormatSize(nxt::VertexFormat format) {
        switch (format) {
            case nxt::VertexFormat::FloatR32G32B32A32:
            case nxt::VertexFormat::FloatR32G32B32:
            case nxt::VertexFormat::FloatR32G32:
                return VertexFormatNumComponents(format) * sizeof(float);
        }
    }

    // InputStateBase

    InputStateBase::InputStateBase(InputStateBuilder* builder) {
        attributesSetMask = builder->attributesSetMask;
        attributeInfos = builder->attributeInfos;
        inputsSetMask = builder->inputsSetMask;
        inputInfos = builder->inputInfos;
    }

    const std::bitset<kMaxVertexAttributes>& InputStateBase::GetAttributesSetMask() const {
        return attributesSetMask;
    }

    const InputStateBase::AttributeInfo& InputStateBase::GetAttribute(uint32_t location) const {
        ASSERT(attributesSetMask[location]);
        return attributeInfos[location];
    }

    const std::bitset<kMaxVertexInputs>& InputStateBase::GetInputsSetMask() const {
        return inputsSetMask;
    }

    const InputStateBase::InputInfo& InputStateBase::GetInput(uint32_t slot) const {
        ASSERT(inputsSetMask[slot]);
        return inputInfos[slot];
    }

    // InputStateBuilder

    InputStateBuilder::InputStateBuilder(DeviceBase* device) : device(device) {
    }

    bool InputStateBuilder::WasConsumed() const {
        return consumed;
    }

    InputStateBase* InputStateBuilder::GetResult() {
        for (uint32_t location = 0; location < kMaxVertexAttributes; ++location) {
            if (attributesSetMask[location] &&
                    !inputsSetMask[attributeInfos[location].bindingSlot]) {
                device->HandleError("Attribute uses unset input");
                return nullptr;
            }
        }
        consumed = true;
        return device->CreateInputState(this);
    }

    void InputStateBuilder::SetAttribute(uint32_t shaderLocation,
            uint32_t bindingSlot, nxt::VertexFormat format, uint32_t offset) {
        if (shaderLocation >= kMaxVertexAttributes) {
            device->HandleError("Setting attribute out of bounds");
            return;
        }
        if (bindingSlot >= kMaxVertexInputs) {
            device->HandleError("Binding slot out of bounds");
            return;
        }
        if (attributesSetMask[shaderLocation]) {
            device->HandleError("Setting already set attribute");
            return;
        }

        attributesSetMask.set(shaderLocation);
        auto& info = attributeInfos[shaderLocation];
        info.bindingSlot = bindingSlot;
        info.format = format;
        info.offset = offset;
    }

    void InputStateBuilder::SetInput(uint32_t bindingSlot, uint32_t stride,
            nxt::InputStepMode stepMode) {
        if (bindingSlot >= kMaxVertexInputs) {
            device->HandleError("Setting input out of bounds");
            return;
        }
        if (inputsSetMask[bindingSlot]) {
            device->HandleError("Setting already set input");
            return;
        }

        inputsSetMask.set(bindingSlot);
        auto& info = inputInfos[bindingSlot];
        info.stride = stride;
        info.stepMode = stepMode;
    }

}
