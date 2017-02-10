//* Copyright (c) 2017 The Chromium Authors. All rights reserved.
//* Use of this source code is governed by a BSD-style license that can be
//* found in the LICENSE file.

#include "mock_nxt.h"

namespace {
    {% for type in by_category["object"] %}
        {% for method in native_methods(type) %}
            {{as_cType(method.return_type.name)}} Forward{{as_MethodSuffix(type.name, method.name)}}(
                {{-as_cType(type.name)}} self
                {%- for arg in method.arguments -%}
                    , {{as_annotated_cType(arg)}}
                {%- endfor -%}
            ) {
                auto tablePtr = reinterpret_cast<ProcTableAsClass**>(self);
                return (*tablePtr)->{{as_MethodSuffix(type.name, method.name)}}(self
                    {%- for arg in method.arguments -%}
                        , {{as_varName(arg.name)}}
                    {%- endfor -%}
                );
            }
        {% endfor %}

    {% endfor %}
}

ProcTableAsClass::~ProcTableAsClass() {
    for (auto ptr : selfPtrs) {
        delete ptr;
    }
}

void ProcTableAsClass::GetProcTableAndDevice(nxtProcTable* table, nxtDevice* device) {
    *device = GetNewDevice();

    {% for type in by_category["object"] %}
        {% for method in native_methods(type) %}
            table->{{as_varName(type.name, method.name)}} = reinterpret_cast<{{as_cProc(type.name, method.name)}}>(Forward{{as_MethodSuffix(type.name, method.name)}});
        {% endfor %}
    {% endfor %}
}

{% for type in by_category["object"] %}
    {{as_cType(type.name)}} ProcTableAsClass::GetNew{{type.name.CamelCase()}}() {
        auto self = new ProcTableAsClass*(this);
        selfPtrs.push_back(self);
        return reinterpret_cast<{{as_cType(type.name)}}>(self);
    }
{% endfor %}
