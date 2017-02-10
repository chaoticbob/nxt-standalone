//* Copyright (c) 2017 The Chromium Authors. All rights reserved.
//* Use of this source code is governed by a BSD-style license that can be
//* found in the LICENSE file.

#ifndef MOCK_NXT_H
#define MOCK_NXT_H

#include <gmock/gmock.h>
#include <nxt/nxt.h>

class ProcTableAsClass {
    public:
        virtual ~ProcTableAsClass();

        void GetProcTableAndDevice(nxtProcTable* table, nxtDevice* device);

        {% for type in by_category["object"] %}
            {{as_cType(type.name)}} GetNew{{type.name.CamelCase()}}();
        {% endfor %}

        {% for type in by_category["object"] %}
            {% for method in native_methods(type) %}
                virtual {{as_cType(method.return_type.name)}} {{as_MethodSuffix(type.name, method.name)}}(
                    {{-as_cType(type.name)}} {{as_varName(type.name)}}
                    {%- for arg in method.arguments -%}
                        , {{as_annotated_cType(arg)}}
                    {%- endfor -%}
                ) = 0;
            {% endfor %}

        {% endfor %}

    private:
        std::vector<ProcTableAsClass**> selfPtrs;
};

class MockProcTable : public ProcTableAsClass {
    public:
        {% for type in by_category["object"] %}
            {% for method in native_methods(type) %}
                MOCK_METHOD{{len(method.arguments) + 1}}(
                    {{-as_MethodSuffix(type.name, method.name)}},
                    {{as_cType(method.return_type.name)}}(
                        {{-as_cType(type.name)}} {{as_varName(type.name)}}
                        {%- for arg in method.arguments -%}
                            , {{as_annotated_cType(arg)}}
                        {%- endfor -%}
                    ));
            {% endfor %}

        {% endfor %}
};

#endif // MOCK_NXT_H
