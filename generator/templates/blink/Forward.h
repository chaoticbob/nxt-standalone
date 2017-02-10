//* Copyright (c) 2017 The Chromium Authors. All rights reserved.
//* Use of this source code is governed by a BSD-style license that can be
//* found in the LICENSE file.

#ifndef NXTForward_H
#define NXTForward_H

namespace blink {

    {% for other_type in by_category["object"] %}
        class NXT{{other_type.name.CamelCase()}};
    {% endfor %}

}

struct nxtProcTable_s;
typedef struct nxtProcTable_s nxtProcTable;

{% for type in by_category["object"] %}
    typedef struct {{as_cType(type.name)}}Impl* {{as_cType(type.name)}};
{% endfor %}

#endif //NXTForward_H
