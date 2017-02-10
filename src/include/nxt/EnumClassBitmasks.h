// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NXT_ENUM_CLASS_BITMASKS_H_
#define NXT_ENUM_CLASS_BITMASKS_H_

#include <type_traits>

namespace nxt {

// std::underlying_type doesn't work in old GLIBC still used in Chrome
#define CR_GLIBCXX_4_7_0 20120322
#define CR_GLIBCXX_4_5_4 20120702
#define CR_GLIBCXX_4_6_4 20121127
#if defined(__GLIBCXX__) &&                                               \
    (__GLIBCXX__ < CR_GLIBCXX_4_7_0 || __GLIBCXX__ == CR_GLIBCXX_4_5_4 || \
     __GLIBCXX__ == CR_GLIBCXX_4_6_4)
#define CR_USE_FALLBACKS_FOR_OLD_GLIBCXX
#endif

#if defined(CR_USE_FALLBACKS_FOR_OLD_GLIBCXX)
    template <typename T>
    struct UnderlyingType {
      using type = __underlying_type(T);
    };
#else
    template <typename T>
    using UnderlyingType = std::underlying_type<T>;
#endif

    template<typename T>
    struct IsNXTBitmask {
        static constexpr bool enable = false;
    };

    template<typename T, typename Enable = void>
    struct LowerBitmask {
        static constexpr bool enable = false;
    };

    template<typename T>
    struct LowerBitmask<T, typename std::enable_if<IsNXTBitmask<T>::enable>::type> {
        static constexpr bool enable = true;
        using type = T;
        static T Lower(T t) {return t;}
    };

    template<typename T>
    struct BoolConvertible {
        using Integral = typename UnderlyingType<T>::type;

        BoolConvertible(Integral value) : value(value) {}
        operator bool() const {return value != 0;}
        operator T() const {return static_cast<T>(value);}

        Integral value;
    };

    template<typename T>
    struct LowerBitmask<BoolConvertible<T>> {
        static constexpr bool enable = true;
        using type = T;
        static type Lower(BoolConvertible<T> t) {return t;}
    };

    template<typename T1, typename T2, typename = typename std::enable_if<
        LowerBitmask<T1>::enable && LowerBitmask<T2>::enable
    >::type>
    constexpr BoolConvertible<typename LowerBitmask<T1>::type> operator | (T1 left, T2 right) {
        using T = typename LowerBitmask<T1>::type;
        using Integral = typename UnderlyingType<T>::type;
        return static_cast<Integral>(LowerBitmask<T1>::Lower(left)) |
               static_cast<Integral>(LowerBitmask<T2>::Lower(right));
    }

    template<typename T1, typename T2, typename = typename std::enable_if<
        LowerBitmask<T1>::enable && LowerBitmask<T2>::enable
    >::type>
    constexpr BoolConvertible<typename LowerBitmask<T1>::type> operator & (T1 left, T2 right) {
        using T = typename LowerBitmask<T1>::type;
        using Integral = typename UnderlyingType<T>::type;
        return static_cast<Integral>(LowerBitmask<T1>::Lower(left)) &
               static_cast<Integral>(LowerBitmask<T2>::Lower(right));
    }

    template<typename T1, typename T2, typename = typename std::enable_if<
        LowerBitmask<T1>::enable && LowerBitmask<T2>::enable
    >::type>
    constexpr BoolConvertible<typename LowerBitmask<T1>::type> operator ^ (T1 left, T2 right) {
        using T = typename LowerBitmask<T1>::type;
        using Integral = typename UnderlyingType<T>::type;
        return static_cast<Integral>(LowerBitmask<T1>::Lower(left)) ^
               static_cast<Integral>(LowerBitmask<T2>::Lower(right));
    }

    template<typename T1>
    constexpr BoolConvertible<typename LowerBitmask<T1>::type> operator ~ (T1 t) {
        using T = typename LowerBitmask<T1>::type;
        using Integral = typename UnderlyingType<T>::type;
        return ~static_cast<Integral>(LowerBitmask<T1>::Lower(t));
    }

    template<typename T, typename T2, typename = typename std::enable_if<
        IsNXTBitmask<T>::enable && LowerBitmask<T2>::enable
    >::type>
    T& operator &= (T& l, T2 right) {
        T r = LowerBitmask<T2>::Lower(right);
        l = l & r;
        return l;
    }

    template<typename T, typename T2, typename = typename std::enable_if<
        IsNXTBitmask<T>::enable && LowerBitmask<T2>::enable
    >::type>
    T& operator |= (T& l, T2 right) {
        T r = LowerBitmask<T2>::Lower(right);
        l = l | r;
        return l;
    }

    template<typename T, typename T2, typename = typename std::enable_if<
        IsNXTBitmask<T>::enable && LowerBitmask<T2>::enable
    >::type>
    T& operator ^= (T& l, T2 right) {
        T r = LowerBitmask<T2>::Lower(right);
        l = l ^ r;
        return l;
    }

}

#endif // NXT_ENUM_CLASS_BITMASKS_H_
