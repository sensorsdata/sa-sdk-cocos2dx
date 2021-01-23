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

#ifndef COCOS2DX_SENSORS_OBJECT_NODE_H_
#define COCOS2DX_SENSORS_OBJECT_NODE_H_

#include <stdio.h>
#include <string>
#include <map>
#include <vector>

using namespace std;

namespace sensorsdata {
    class ObjectNode {
    public:
        void setNumber(const char *propertyName, int32_t value);

        void setNumber(const char *propertyName, int64_t value);

        void setNumber(const char *propertyName, double value);

        void setString(const char *propertyName, const char *value);

        void setBool(const char *propertyName, bool value);

        void setList(const char *propertyName, const std::vector<string> &value);

        void setDateTime(const char *propertyName, time_t seconds, int milliseconds);

        /**
         * 设置时间为事件属性
         * @param property_name 属性名
         * @param value 时间字符串，格式需要是：2020-12-31 16:30:27.567
         */
        void setDateTime(const char *propertyName, const char *value);

        void clear();

        static string toJson(const ObjectNode &node);

        void mergeFrom(const ObjectNode &anotherNode);

        class ValueNode;

        std::map<string, ValueNode> propertiesMap;

    private:
        static void dumpNode(const ObjectNode &node, string *buffer);

        enum ValueNodeType {
            NUMBER,
            INT,
            STRING,
            LIST,
            DATETIME,
            BOOL,
            UNKNOWN,
        };
    };

    class ObjectNode::ValueNode {
    public:
        ValueNode() : nodeType(UNKNOWN) {}

        explicit ValueNode(double value);

        explicit ValueNode(int64_t value);

        explicit ValueNode(const string &value);

        explicit ValueNode(bool value);

        explicit ValueNode(const ObjectNode &value);

        explicit ValueNode(const std::vector<string> &value);

        ValueNode(time_t seconds, int milliseconds);

        static void toStr(const ValueNode &node, string *buffer);

    private:
        static void dumpString(const string &value, string *buffer);

        static void dumpList(const std::vector<string> &value, string *buffer);

        static void dumpDateTime(const time_t &seconds, int milliseconds, string *buffer);

        static void dumpNumber(double value, string *buffer);

        static void dumpNumber(int64_t value, string *buffer);

        ValueNodeType nodeType;

        union UnionValue {
            double numberValue;
            bool boolValue;
            struct {
                time_t seconds;
                int milliseconds;
            } datetimeValue;
            int64_t intValue;

            UnionValue() { memset(this, 0, sizeof(UnionValue)); }
        } valueData;

        string stringData;
        std::vector<string> listData;
        ObjectNode objectData;
    };
}

#endif // COCOS2DX_SENSORS_OBJECT_NODE_HPP_
