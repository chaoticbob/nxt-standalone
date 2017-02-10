//* Copyright (c) 2017 The Chromium Authors. All rights reserved.
//* Use of this source code is governed by a BSD-style license that can be
//* found in the LICENSE file.

#include "Wire.h"

#include <cstring>
#include <vector>

//* Definition of the wire format for the commands, and helper methods to compute their wire size.
namespace wire {

    //* Enum used to know which command is which.
    enum class WireCmd : uint32_t {
        {% for type in by_category["object"] %}
            {% for method in type.methods %}
                {{as_MethodSuffix(type.name, method.name)}},
            {% endfor %}
            {{as_MethodSuffix(type.name, Name("destroy"))}},
        {% endfor %}
    };

    {% for type in by_category["object"] %}
        {% for method in type.methods %}
            {% set Suffix = as_MethodSuffix(type.name, method.name) %}

            //* Structure for the wire format of each of the commands
            struct {{Suffix}}Cmd {

                //* Start the structure with the command ID, so that casting to WireCmd gives the ID.
                wire::WireCmd commandId = wire::WireCmd::{{Suffix}};

                uint32_t self;

                //* Commands creating objects say which ID the created object will be referred as.
                {% if method.return_type.category == "object" %}
                    uint32_t resultId;
                {% endif %}

                //* Value types are directly in the command, objects being replaced with their IDs.
                {% for arg in method.arguments if arg.annotation == "value" %}
                    {% if arg.type.category == "object" %}
                        uint32_t {{as_varName(arg.name)}};
                    {% else %}
                        {{as_cType(arg.type.name)}} {{as_varName(arg.name)}};
                    {% endif %}
                {% endfor %}

                //* const char* have their length directly in the command.
                {% for arg in method.arguments if arg.length == "strlen" %}
                    size_t {{as_varName(arg.name)}}Strlen;
                {% endfor %}

                //* For a filled command (all value params initialized), get how much wire space is needed to send it
                size_t GetRequiredSize() const {
                    size_t result = sizeof(*this);
                    {% for arg in method.arguments if arg.annotation != "value" %}
                        {% if arg.length == "strlen" %}
                            result += {{as_varName(arg.name)}}Strlen + 1;
                        {% elif arg.type.category == "object" %}
                            result += {{as_varName(arg.length.name)}} * sizeof(uint32_t);
                        {% else %}
                            result += {{as_varName(arg.length.name)}} * sizeof({{as_cType(arg.type.name)}});
                        {% endif %}
                    {% endfor %}
                    return result;
                }

                //* For a filled command (all value params initialized), get the offset to buffers for non-value params.
                {% for const in ["", "const"] %}
                    {% for get_arg in method.arguments if get_arg.annotation != "value" %}
                        {{const}} uint8_t* GetPtr_{{as_varName(get_arg.name)}}() {{const}} {
                            {{const}} uint8_t* ptr = reinterpret_cast<{{const}} uint8_t*>(this + 1);
                            {% for arg in method.arguments if arg.annotation != "value" %}
                                {% if get_arg == arg %}
                                    return ptr;
                                {% endif %}
                                {% if arg.length == "strlen" %}
                                    ptr += {{as_varName(arg.name)}}Strlen + 1;
                                {% elif arg.type.category == "object" %}
                                    ptr += {{as_varName(arg.length.name)}} * sizeof(uint32_t);
                                {% else %}
                                    ptr += {{as_varName(arg.length.name)}} * sizeof({{as_cType(arg.type.name)}});
                                {% endif %}
                            {% endfor %}
                        }
                    {% endfor %}
                {% endfor %}
            };
        {% endfor %}

        //* The command structure used when sending that an ID is destroyed.
        {% set Suffix = as_MethodSuffix(type.name, Name("destroy")) %}
        struct {{Suffix}}Cmd {
            wire::WireCmd commandId = wire::WireCmd::{{Suffix}};
            uint32_t objectId;

            size_t GetRequiredSize() const {
                return sizeof(*this);
            }
        };
    {% endfor %}
}

//* Client side implementation of the API, will serialize everything to memory to send to the server side.
namespace client {

    class Device;

    //* All non-Device objects of the client side have:
    //*  - A pointer to the device to get where to serialize commands
    //*  - The external reference count
    //*  - An ID that is used to refer to this object when talking with the server side
    {% for type in by_category["object"] if not type.name.canonical_case() == "device" %}
        struct {{type.name.CamelCase()}} {
            Device* device;
            uint32_t refcount;
            uint32_t id;
        };
    {% endfor %}

    //* TODO
    //*  - Remember objects so they can all be destroyed at device destruction.
    class IdAllocator {
        public:
            uint32_t GetId() {
                if (freeIds.empty()) {
                    return currentId ++;
                }
                uint32_t id = freeIds.back();
                freeIds.pop_back();
                return id;
            }
            void FreeId(uint32_t id) {
                freeIds.push_back(id);
            };

        private:
            uint32_t currentId = 1;
            std::vector<uint32_t> freeIds;
    };

    //* The global data of the client API implementation, handles ID allocation, memory management, and sending commands.
    class Device {
        public:
            Device(CommandSerializer* serializer) : device(this), serializer(serializer) {
            }

            Device* device = nullptr;

            uint32_t id = 0;

            void* GetCmdSpace(size_t size) {
                return serializer->GetCmdSpace(size);
            }

        private:
           CommandSerializer* serializer = nullptr;

        {% for type in by_category["object"] if not type.name.canonical_case() == "device" %}
            {% set Type = type.name.CamelCase() %}
            public:
                {{Type}}* Get{{Type}}() {
                    auto obj = new {{Type}};
                    obj->device = this;
                    obj->refcount = 1;
                    obj->id = idAllocator{{Type}}.GetId();
                    return obj;
                }
                void Free{{Type}}({{Type}}* obj) {
                    idAllocator{{Type}}.FreeId(obj->id);
                    delete obj;
                }

            private:
                IdAllocator idAllocator{{Type}};
        {% endfor %}
    };

    //* Implementation of the client API functions.
    {% for type in by_category["object"] %}
        {% set Type = type.name.CamelCase() %}

        {% for method in type.methods %}
            {% set Suffix = as_MethodSuffix(type.name, method.name) %}

            {{as_backendType(method.return_type)}} Client{{Suffix}}(
                {{-as_backendType(type)}} self
                {%- for arg in method.arguments -%}
                    , {{as_annotated_backendType(arg)}}
                {%- endfor -%}
            ) {
                //* Create the structure going on the wire, fill it with the value arguments
                wire::{{Suffix}}Cmd cmd;
                {% for arg in method.arguments if arg.annotation == "value" %}
                    {% if arg.type.category == "object" %}
                        cmd.{{as_varName(arg.name)}} = {{as_varName(arg.name)}}->id;
                    {% else %}
                        cmd.{{as_varName(arg.name)}} = {{as_varName(arg.name)}};
                    {% endif %}
                {% endfor %}

                cmd.self = self->id;

                //* The length of const char* is considered a value argument.
                {% for arg in method.arguments if arg.length == "strlen" %}
                    cmd.{{as_varName(arg.name)}}Strlen = strlen({{as_varName(arg.name)}});
                {% endfor %}

                //* Allocated space to send the command.
                size_t requiredSize = cmd.GetRequiredSize();
                auto allocCmd = reinterpret_cast<wire::{{Suffix}}Cmd*>(self->device->GetCmdSpace(requiredSize));
                *allocCmd = cmd;

                //* In that space, write the non-value arguments.
                {% for arg in method.arguments if arg.annotation != "value" %}
                    {% set argName = as_varName(arg.name) %}
                    {% if arg.length == "strlen" %}
                        memcpy(allocCmd->GetPtr_{{argName}}(), {{argName}}, allocCmd->{{argName}}Strlen + 1);
                    {% elif arg.type.category == "object" %}
                        auto {{argName}}Storage = reinterpret_cast<uint32_t*>(allocCmd->GetPtr_{{argName}}());
                        for (size_t i = 0; i < {{as_varName(arg.length.name)}}; i++) {
                            {{argName}}Storage[i] = {{argName}}[i]->id;
                        }
                    {% else %}
                        memcpy(allocCmd->GetPtr_{{argName}}(), {{argName}}, {{as_varName(arg.length.name)}} * sizeof(*{{argName}}));
                    {% endif %}
                {% endfor %}

                //* For object creation, we need to say which ID the object will be referred to in the command.
                {% if method.return_type.category == "object" %}
                    auto result = self->device->Get{{method.return_type.name.CamelCase()}}();
                    allocCmd->resultId = result->id;
                    return result;
                {% endif %}
            }
        {% endfor %}

        {% if not type.name.canonical_case() == "device" %}
            //* When an object's refcount reaches 0, notify the server side of it.
            void Client{{as_MethodSuffix(type.name, Name("release"))}}({{Type}}* obj) {
                obj->refcount --;
                if (obj->refcount == 0) {
                    wire::{{as_MethodSuffix(type.name, Name("destroy"))}}Cmd cmd;
                    cmd.objectId = obj->id;
                    auto allocCmd = reinterpret_cast<wire::{{as_MethodSuffix(type.name, Name("destroy"))}}Cmd*>(obj->device->GetCmdSpace(cmd.GetRequiredSize()));
                    *allocCmd = cmd;

                    obj->device->Free{{Type}}(obj);
                }
            }

            void Client{{as_MethodSuffix(type.name, Name("reference"))}}({{Type}}* obj) {
                obj->refcount ++;
            }
        {% endif %}
    {% endfor %}

    void ClientDeviceReference(client::Device* self) {
    }

    void ClientDeviceRelease(client::Device* self) {
    }

    nxtProcTable GetProcs() {
        nxtProcTable table;
        {% for type in by_category["object"] %}
            {% for method in native_methods(type) %}
                table.{{as_varName(type.name, method.name)}} = reinterpret_cast<{{as_cProc(type.name, method.name)}}>(Client{{as_MethodSuffix(type.name, method.name)}});
            {% endfor %}
        {% endfor %}
        return table;
    }

    void NewClientDevice(nxtProcTable* procs, nxtDevice* device, CommandSerializer* serializer) {
        *device = reinterpret_cast<nxtDeviceImpl*>(new Device(serializer));
        *procs = GetProcs();
    }
}

namespace server {
    {% for type in by_category["object"] %}
        struct {{type.name.CamelCase()}}Data {
            {{as_cType(type.name)}} ptr;
            bool valid;
            bool allocated;
        };
    {% endfor %}

    class Server : public CommandHandler {
        public:
            Server(nxtDevice device, const nxtProcTable& procs) : procs(procs) {
                deviceData.ptr = device;
                deviceData.allocated = true;
                deviceData.valid = true;

                {% for type in by_category["object"] if not type.name.canonical_case() == "device" %}
                    {% set Type = type.name.CamelCase() %}
                    {
                        {{Type}}Data nullObject;
                        nullObject.ptr = nullptr;
                        nullObject.valid = true;
                        nullObject.allocated = true;
                        known{{Type}}.push_back(nullObject);
                    }
                {% endfor %}
            }

            const uint8_t* HandleCommands(const uint8_t* commands, size_t size) override {
                while (size > 0) {
                    wire::WireCmd cmdId = *reinterpret_cast<const wire::WireCmd*>(commands);

                    bool success = false;
                    switch (cmdId) {
                        {% for type in by_category["object"] %}
                            {% for method in type.methods %}
                                {% set Suffix = as_MethodSuffix(type.name, method.name) %}
                                case wire::WireCmd::{{Suffix}}:
                                    success = Handle{{Suffix}}(&commands, &size);
                                    break;
                            {% endfor %}
                            {% set Suffix = as_MethodSuffix(type.name, Name("destroy")) %}
                            case wire::WireCmd::{{Suffix}}:
                                success = Handle{{Suffix}}(&commands, &size);
                                break;
                        {% endfor %}

                        default:
                            success = false;
                    }

                    if (!success) {
                        return nullptr;
                    }
                }
                return commands;
            }

            void OnSynchronousError() override {
                gotError = true;
            }

        private:
            nxtProcTable procs;
            DeviceData deviceData;
            bool gotError = false;

            {% for type in by_category["object"] %}
                {% for method in type.methods %}
                    {% set Suffix = as_MethodSuffix(type.name, method.name) %}
                    bool Handle{{Suffix}}(const uint8_t** commands, size_t* size) {
                        //* Get command ptr, and check it fits in the buffer.
                        if (*size < sizeof(wire::{{Suffix}}Cmd)) {
                            return false;
                        }

                        const wire::{{Suffix}}Cmd* cmd = reinterpret_cast<const wire::{{Suffix}}Cmd*>(*commands);
                        size_t cmdSize = cmd->GetRequiredSize();
                        if (*size < cmdSize) {
                            return false;
                        }

                        *commands += cmdSize;
                        *size -= cmdSize;

                        bool valid = true;

                        //* Unpack 'self'
                        {% set Type = type.name.CamelCase() %}
                        {{as_cType(type.name)}} self;
                        {{Type}}Data* selfData = Get{{Type}}Data(cmd->self);
                        {
                            if (selfData == nullptr) {
                                return false;
                            }
                            valid = valid && selfData->valid;
                            self = selfData->ptr;
                        }

                        //* Unpack value objects from IDs.
                        {% for arg in method.arguments if arg.annotation == "value" and arg.type.category == "object" %}
                            {% set Type = arg.type.name.CamelCase() %}
                            {{as_cType(arg.type.name)}} arg_{{as_varName(arg.name)}};
                            {
                                {{Type}}Data* data = Get{{Type}}Data(cmd->{{as_varName(arg.name)}});
                                if (data == nullptr) {
                                    return false;
                                }
                                valid = valid && data->valid;
                                arg_{{as_varName(arg.name)}} = data->ptr;
                            }
                        {% endfor %}

                        {% for arg in method.arguments if arg.annotation != "value" %}
                            {% set argName = as_varName(arg.name) %}
                            const {{as_cType(arg.type.name)}}* arg_{{argName}};
                            {% if arg.length == "strlen" %}
                                //* Unpack strings, checking they are null-terminated.
                                arg_{{argName}} = reinterpret_cast<const {{as_cType(arg.type.name)}}*>(cmd->GetPtr_{{argName}}());
                                if (arg_{{argName}}[cmd->{{argName}}Strlen] != 0) {
                                    return false;
                                }
                            {% elif arg.type.category == "object" %}
                                //* Unpack arrays of objects.
                                std::vector<{{as_cType(arg.type.name)}}> {{argName}}Storage(cmd->{{as_varName(arg.length.name)}});
                                auto {{argName}}Ids = reinterpret_cast<const uint32_t*>(cmd->GetPtr_{{argName}}());
                                for (size_t i = 0; i < cmd->{{as_varName(arg.length.name)}}; i++) {
                                    {% set Type = arg.type.name.CamelCase() %}
                                    {{Type}}Data* data = Get{{Type}}Data({{argName}}Ids[i]);
                                    if (data == nullptr) {
                                        return false;
                                    }
                                    {{argName}}Storage[i] = data->ptr;
                                    valid = valid && data->valid;
                                }
                                arg_{{argName}} = {{argName}}Storage.data();
                            {% else %}
                                //* For anything else, just get the pointer.
                                arg_{{argName}} = reinterpret_cast<const {{as_cType(arg.type.name)}}*>(cmd->GetPtr_{{argName}}());
                            {% endif %}
                        {% endfor %}

                        {% set returns = method.return_type.name.canonical_case() != "void" %}
                        {% if returns %}
                            {% set Type = method.return_type.name.CamelCase() %}
                            {{Type}}Data* resultData = GetNew{{Type}}Data(cmd->resultId);
                            if (resultData == nullptr) {
                                return false;
                            }
                        {% endif %}

                        if (valid) {
                            {% if returns -%}
                                auto result =
                            {%- endif -%}
                            procs.{{as_varName(type.name, method.name)}}(self
                                {%- for arg in method.arguments -%}
                                    {% if arg.annotation == "value" and arg.type.category != "object" %}
                                        , cmd->{{as_varName(arg.name)}}
                                    {% else %}
                                        , arg_{{as_varName(arg.name)}}
                                    {% endif %}
                                {%- endfor -%}
                            );

                            {% if returns %}
                                resultData->ptr = result;
                                resultData->valid = result != nullptr;
                            {% endif %}
                            if (gotError) {
                                {% if type.is_builder %}
                                    selfData->valid = false;
                                {% endif %}
                                gotError = false;
                            }
                        } else {
                            {% if returns %}
                                resultData->ptr = nullptr;
                                resultData->valid = false;
                            {% endif %}
                        }

                        return true;
                    }
                {% endfor %}
                {% set Suffix = as_MethodSuffix(type.name, Name("destroy")) %}
                    bool Handle{{Suffix}}(const uint8_t** commands, size_t* size) {
                        if (*size < sizeof(wire::{{Suffix}}Cmd)) {
                            return false;
                        }

                        const wire::{{Suffix}}Cmd* cmd = reinterpret_cast<const wire::{{Suffix}}Cmd*>(*commands);
                        size_t cmdSize = cmd->GetRequiredSize();
                        if (*size < cmdSize) {
                            return false;
                        }

                        *commands += cmdSize;
                        *size -= cmdSize;

                        //* Unpack 'self'
                        {% set Type = type.name.CamelCase() %}
                        if (cmd->objectId == 0) {
                            return false;
                        }

                        {{Type}}Data* data = Get{{Type}}Data(cmd->objectId);
                        if (data == nullptr) {
                            return false;
                        }

                        if (data->valid) {
                            procs.{{as_varName(type.name, Name("release"))}}(data->ptr);
                        }

                        data->allocated = false;
                        return true;
                    }
            {% endfor %}

            {% for type in by_category["object"] if not type.name.canonical_case() == "device" %}
                {% set Type = type.name.CamelCase() %}
                std::vector<{{Type}}Data> known{{Type}};
                {{Type}}Data* Get{{Type}}Data(uint32_t id) {
                    if (id >= known{{Type}}.size()) {
                        return nullptr;
                    }
                    {{Type}}Data* data = &known{{Type}}[id];
                    if (!data->allocated) {
                        return nullptr;
                    }
                    return data;
                }
                {{Type}}Data* GetNew{{Type}}Data(uint32_t id) {
                    if (id > known{{Type}}.size()) {
                        return nullptr;
                    }

                    {{Type}}Data data;
                    data.allocated = true;
                    data.valid = false;
                    data.ptr = nullptr;

                    if (id >= known{{Type}}.size()) {
                        known{{Type}}.push_back(data);
                        return &known{{Type}}.back();
                    }

                    if (known{{Type}}[id].allocated) {
                        return nullptr;
                    }

                    known{{Type}}[id] = data;
                    return &known{{Type}}[id];
                }

            {% endfor %}

            DeviceData* GetDeviceData(uint32_t id) {
                if (id == 0) {
                    return &deviceData;
                } else {
                    return nullptr;
                }
            }
    };

    CommandHandler* CreateCommandHandler(nxtDevice device, const nxtProcTable& procs) {
        return new Server(device, procs);
    }
}
