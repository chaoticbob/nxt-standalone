// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BACKEND_COMMON_REFCOUNTED_H_
#define BACKEND_COMMON_REFCOUNTED_H_

#include <cstdint>

namespace backend {

    class RefCounted {
        public:
            RefCounted();
            virtual ~RefCounted();

            void ReferenceInternal();
            void ReleaseInternal();

            uint32_t GetExternalRefs() const;
            uint32_t GetInternalRefs() const;

            // NXT API
            void Reference();
            void Release();

        protected:
            uint32_t externalRefs = 1;
            uint32_t internalRefs = 1;
    };

    template<typename T>
    class Ref {
        public:
            Ref() {}

            Ref(T* p): pointee(p) {
                Reference();
            }

            Ref(Ref<T>& other): pointee(other.pointee) {
                Reference();
            }
            Ref<T>& operator=(const Ref<T>& other) {
                if (&other == this) return *this;

                other.Reference();
                Release();
                pointee = other.pointee;

                return *this;
            }

            Ref(Ref<T>&& other) {
                pointee = other.pointee;
                other.pointee = nullptr;
            }
            Ref<T>& operator=(Ref<T>&& other) {
                if (&other == this) return *this;

                Release();
                pointee = other.pointee;
                other.pointee = nullptr;

                return *this;
            }

            ~Ref() {
                Release();
                pointee = nullptr;
            }

            operator bool() {
                return pointee != nullptr;
            }

            const T& operator*() const {
                return *pointee;
            }
            T& operator*() {
                return *pointee;
            }

            const T* operator->() const {
                return pointee;
            }
            T* operator->() {
                return pointee;
            }

            const T* Get() const {
                return pointee;
            }
            T* Get() {
                return pointee;
            }

        private:
            void Reference() const {
                if (pointee != nullptr) {
                    pointee->ReferenceInternal();
                }
            }
            void Release() const {
                if (pointee != nullptr) {
                    pointee->ReleaseInternal();
                }
            }

            //static_assert(std::is_base_of<RefCounted, T>::value, "");
            T* pointee = nullptr;
    };

}

#endif // BACKEND_COMMON_REFCOUNTED_H_
