//* Copyright (c) 2017 The Chromium Authors. All rights reserved.
//* Use of this source code is governed by a BSD-style license that can be
//* found in the LICENSE file.

#include "nxtcpp.h"

namespace nxt {

    {% for type in by_category["enum"] + by_category["bitmask"] %}
        {% set CppType = as_cppType(type.name) %}
        {% set CType = as_cType(type.name) %}

        static_assert(sizeof({{CppType}}) == sizeof({{CType}}), "sizeof mismatch for {{CppType}}");
        static_assert(alignof({{CppType}}) == alignof({{CType}}), "alignof mismatch for {{CppType}}");

        {% for value in type.values %}
            static_assert(static_cast<uint32_t>({{CppType}}::{{as_cppEnum(value.name)}}) == {{as_cEnum(type.name, value.name)}}, "value mismatch for {{CppType}}::{{as_cppEnum(value.name)}}");
        {% endfor %}

    {% endfor %}


    {% for type in by_category["object"] %}
        {% set CppType = as_cppType(type.name) %}
        {% set CType = as_cType(type.name) %}

        static_assert(sizeof({{CppType}}) == sizeof({{CType}}), "sizeof mismatch for {{CppType}}");
        static_assert(alignof({{CppType}}) == alignof({{CType}}), "alignof mismatch for {{CppType}}");

        {% macro render_cpp_method_declaration(type, method) %}
            {% set CppType = as_cppType(type.name) %}
            {% if method.return_type.name.concatcase() == "void" -%}
                {{CppType}} const&
            {%- else -%}
                {{as_cppType(method.return_type.name)}}
            {%- endif -%}
            {{" "}}{{CppType}}::{{method.name.CamelCase()}}(
                {%- for arg in method.arguments -%}
                    {%- if not loop.first %}, {% endif -%}
                    {%- if arg.type.category == "object" and arg.annotation == "value" -%}
                        {{as_cppType(arg.type.name)}} const& {{as_varName(arg.name)}}
                    {%- else -%}
                        {{as_annotated_cppType(arg)}}
                    {%- endif -%}
                {%- endfor -%}
            ) const
        {%- endmacro %}

        {% macro render_cpp_to_c_method_call(type, method) -%}
            {{as_cMethod(type.name, method.name)}}(Get()
                {%- for arg in method.arguments -%},{{" "}}
                    {%- if arg.annotation == "value" -%}
                        {%- if arg.type.category == "object" -%}
                            {{as_varName(arg.name)}}.Get()
                        {%- elif arg.type.category == "enum" or arg.type.category == "bitmask" -%}
                            static_cast<{{as_cType(arg.type.name)}}>({{as_varName(arg.name)}})
                        {%- elif arg.type.category == "native" -%}
                            {{as_varName(arg.name)}}
                        {%- else -%}
                            UNHANDLED
                        {%- endif -%}
                    {%- else -%}
                        reinterpret_cast<{{decorate("", as_cType(arg.type.name), arg)}}>({{as_varName(arg.name)}})
                    {%- endif -%}
                {%- endfor -%}
            )
        {%- endmacro %}

        {% for method in type.methods %}
            {{render_cpp_method_declaration(type, method)}} {
                {% if method.return_type.name.concatcase() == "void" %}
                    {{render_cpp_to_c_method_call(type, method)}};
                    return *this;
                {% else %}
                    auto result = {{render_cpp_to_c_method_call(type, method)}};
                    {% if method.return_type.category == "native" %} 
                        return result;
                    {% elif method.return_type.category == "object" %}
                        return {{as_cppType(method.return_type.name)}}::Acquire(result);
                    {% else %}
                        return static_cast<{{as_cppType(method.return_type.name)}}>(result);
                    {% endif%}
                {% endif %}
            }
        {% endfor %}
        void {{CppType}}::NxtReference({{CType}} handle) {
            if (handle != nullptr) {
                {{as_cMethod(type.name, Name("reference"))}}(handle);
            }
        }
        void {{CppType}}::NxtRelease({{CType}} handle) {
            if (handle != nullptr) {
                {{as_cMethod(type.name, Name("release"))}}(handle);
            }
        }

    {% endfor %}

}
