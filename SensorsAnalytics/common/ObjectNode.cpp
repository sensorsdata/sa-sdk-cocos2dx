/*
 * Created by yuejz on 2020/12/24.
 * Copyright 2015－2020 Sensors Data Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "../include/ObjectNode.h"
#include <sstream>

using namespace sensorsdata;

void ObjectNode::setNumber(const char *propertyName, double value) {
    if (!propertyName) return;
    propertiesMap[string(propertyName)] = ValueNode(value);
}

void ObjectNode::setNumber(const char *propertyName, int32_t value) {
    if (!propertyName) return;
    propertiesMap[string(propertyName)] = ValueNode(static_cast<int64_t>(value));
}

void ObjectNode::setNumber(const char *propertyName, int64_t value) {
    if (!propertyName) return;
    propertiesMap[string(propertyName)] = ValueNode(value);
}

void ObjectNode::setString(const char *propertyName, const char *value) {
    if (!propertyName || !value) return;
    propertiesMap[string(propertyName)] = ValueNode(string(value));
}

void ObjectNode::setBool(const char *propertyName, bool value) {
    if (!propertyName) return;
    propertiesMap[string(propertyName)] = ValueNode(value);
}

void ObjectNode::setList(const char *propertyName, const std::vector<string> &value) {
    if (!propertyName) return;
    propertiesMap[string(propertyName)] = ValueNode(value);
}

void ObjectNode::setDateTime(const char *propertyName, const time_t seconds, int milliseconds) {
    if (!propertyName) return;
    propertiesMap[string(propertyName)] = ValueNode(seconds, milliseconds);
}

void ObjectNode::setDateTime(const char *propertyName, const char *value) {
    if (!propertyName || !value) return;
    propertiesMap[string(propertyName)] = ValueNode(string(value));
}

void ObjectNode::clear() {
    propertiesMap.clear();
}

void ObjectNode::dumpNode(const ObjectNode &node, string *buffer) {
    *buffer += '{';
    bool first = true;

    for (std::map<string, ValueNode>::const_iterator iterator = node.propertiesMap.begin(); iterator != node.propertiesMap.end(); ++iterator) {
        if (first) {
            first = false;
        } else {
            *buffer += ',';
        }
        *buffer += '"' + iterator->first + "\":";
        ValueNode::toStr(iterator->second, buffer);
    }
    *buffer += '}';
}

void ObjectNode::ValueNode::dumpString(const string &value, string *buffer) {
    *buffer += '"';
    for (std::string::size_type i = 0; i < value.length(); ++i) {
        char c = value[i];
        switch (c) {
            case '"':
                *buffer += "\\\"";
                break;
            case '\\':
                *buffer += "\\\\";
                break;
            case '\b':
                *buffer += "\\b";
                break;
            case '\f':
                *buffer += "\\f";
                break;
            case '\n':
                *buffer += "\\n";
                break;
            case '\r':
                *buffer += "\\r";
                break;
            case '\t':
                *buffer += "\\t";
                break;
            default:
                *buffer += c;
                break;
        }
    }
    *buffer += '"';
}

void ObjectNode::ValueNode::dumpList(const std::vector<string> &value, string *buffer) {
    *buffer += '[';
    bool first = true;
    for (std::vector<string>::const_iterator iterator = value.begin(); iterator != value.end(); ++iterator) {
        if (first) {
            first = false;
        } else {
            *buffer += ',';
        }
        dumpString(*iterator, buffer);
    }
    *buffer += ']';
}

#if defined(__linux__)
#define SA_SDK_LOCALTIME(seconds, now) localtime_r((seconds), (now))
#elif defined(__APPLE__)
#define SA_SDK_LOCALTIME(seconds, now) localtime_r((seconds), (now))
#elif defined(_WIN32)
#define SA_SDK_LOCALTIME(seconds, now) localtime_s((now), (seconds))
#define snprintf sprintf_s
#endif

void ObjectNode::ValueNode::dumpDateTime(const time_t &seconds, int milliseconds, string *buffer) {
    struct tm tm = {};
    SA_SDK_LOCALTIME(&seconds, &tm);
    char buff[64];
    snprintf(buff, sizeof(buff), "\"%04d-%02d-%02d %02d:%02d:%02d.%03d\"",
             tm.tm_year + 1900,
             tm.tm_mon + 1,
             tm.tm_mday,
             tm.tm_hour,
             tm.tm_min,
             tm.tm_sec,
             milliseconds);
    *buffer += buff;
}

string ObjectNode::toJson(const ObjectNode &node) {
    string buffer;
    dumpNode(node, &buffer);
    return buffer;
}

ObjectNode::ValueNode::ValueNode(double value) : nodeType(NUMBER) {
    valueData.numberValue = value;
}

ObjectNode::ValueNode::ValueNode(int64_t value) : nodeType(INT) {
    valueData.intValue = value;
}

ObjectNode::ValueNode::ValueNode(const string &value) : nodeType(STRING), stringData(value) {}

ObjectNode::ValueNode::ValueNode(bool value) : nodeType(BOOL) {
    valueData.boolValue = value;
}

ObjectNode::ValueNode::ValueNode(const std::vector<string> &value) : nodeType(LIST), listData(value) {}

ObjectNode::ValueNode::ValueNode(time_t seconds, int milliseconds) : nodeType(DATETIME) {
    valueData.datetimeValue.seconds = seconds;
    valueData.datetimeValue.milliseconds = milliseconds;
}

void ObjectNode::ValueNode::toStr(const ObjectNode::ValueNode &node, string *buffer) {
    switch (node.nodeType) {
        case NUMBER:
            dumpNumber(node.valueData.numberValue, buffer);
            break;
        case INT:
            dumpNumber(node.valueData.intValue, buffer);
            break;
        case STRING:
            dumpString(node.stringData, buffer);
            break;
        case LIST:
            dumpList(node.listData, buffer);
            break;
        case BOOL:
            *buffer += (node.valueData.boolValue ? "true" : "false");
            break;
        case DATETIME:
            dumpDateTime(node.valueData.datetimeValue.seconds,
                         node.valueData.datetimeValue.milliseconds, buffer);
            break;
        default:
            break;
    }
}

void ObjectNode::ValueNode::dumpNumber(double value, string *buffer) {
    std::ostringstream buf;
    buf << value;
    *buffer += buf.str();
}

void ObjectNode::ValueNode::dumpNumber(int64_t value, string *buffer) {
    std::ostringstream buf;
    buf << value;
    *buffer += buf.str();
}

void ObjectNode::mergeFrom(const ObjectNode &anotherNode) {
    for (std::map<string, ValueNode>::const_iterator
                 iterator = anotherNode.propertiesMap.begin();
         iterator != anotherNode.propertiesMap.end(); ++iterator) {
        propertiesMap[iterator->first] = iterator->second;
    }
}
