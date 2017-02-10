//* Copyright (c) 2017 The Chromium Authors. All rights reserved.
//* Use of this source code is governed by a BSD-style license that can be
//* found in the LICENSE file.

#include "nxt/nxt.h"

static nxtProcTable procs;

static nxtProcTable nullProcs;

void nxtSetProcs(const nxtProcTable* procs_) {
    if (procs_) {
        procs = *procs_;
    } else {
        procs = nullProcs;
    }
}

{% for type in by_category["object"] %}
    {% for method in native_methods(type) %}
        {{as_cType(method.return_type.name)}} {{as_cMethod(type.name, method.name)}}(
            {{-as_cType(type.name)}} {{as_varName(type.name)}}
            {%- for arg in method.arguments -%}
                , {{as_annotated_cType(arg)}}
            {%- endfor -%}
        ) {
            {% if method.return_type.name.canonical_case() != "void" %}return {% endif %}
            procs.{{as_varName(type.name, method.name)}}({{as_varName(type.name)}}
                {%- for arg in method.arguments -%}
                    , {{as_varName(arg.name)}}
                {%- endfor -%}
            );
        }
    {% endfor %}

{% endfor %}
