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

#ifndef COCOS2DX_SENSORS_ANALYTICS_H_
#define COCOS2DX_SENSORS_ANALYTICS_H_

#include "ObjectNode.h"
#include "FlushPolicy.h"

#define SENSORS_ANALYTICS_PLUGIN_VERSION_KEY "$lib_plugin_version"
#define SENSORS_ANALYTICS_PLUGIN_VERSION_VALUE "cocos2dx:0.0.1"

namespace sensorsdata {
    class SensorsAnalytics {

    public:
        
        /**
         * 自定义匿名 ID
         * @param anonymousId 匿名 ID
         */
        static void identify(const char *anonymousId);

        /**
         * 追踪事件
         * @param eventName 事件名
         */
        static void track(const char *eventName);

        /**
         * 追踪一个带有属性的事件
         * @param eventName 事件名
         * @param properties 事件属性
         */
        static void track(const char* eventName, const ObjectNode &properties);

        /**
         * 设置当前用户的登录 ID
         * @param loginId 登录 ID
         */
        static void login(const char *loginId);

        /**
         * 清空当前用户的登录 ID
         */
        static void logout();

        /**
         * 设置用户属性
         * @param properties 用户属性
         */
        static void profileSet(const ObjectNode &properties);

        /**
         * 获取公共属性
         * @return 公共属性
         */
        static string getSuperProperties();

        /**
         * 强制上传数据
         */
        static void flush();

        /**
         * 开始事件计时
         * @param eventName 事件名
         * @return 交叉计时的事件名
         */
        static string trackTimerStart(const char *eventName);

        /**
         * 暂停事件计时
         * @param eventName 事件名
         */
        static void trackTimerPause(const char *eventName);

        /**
         * 恢复事件计时
         * @param eventName 事件名
         */
        static void trackTimerResume(const char *eventName);
        
        /**
         * 结束事件计时
         * @param eventName 事件名
         */
        static void trackTimerEnd(const char *eventName);

        /**
         * 结束事件计时
         * @param eventName 事件名
         * @param properties 事件属性
         */
        static void trackTimerEnd(const char *eventName, const ObjectNode &properties);

        /**
         * 清除事件计时器
         */
        static void clearTrackTimer();

        /**
         * 注册公共属性
         * @param properties 事件属性
         */
        static void registerSuperProperties(const ObjectNode &properties);

        /**
         * 从公共属性中删除某个属性
         * @param superPropertyName 删除的属性
         */
        static void unregisterSuperProperty(const char *superPropertyName);

        /**
         * 清空公共属性
         */
        static void clearSuperProperties();

        /**
         * 设置数据上传网络策略
         * @param types 网络策略
         */
        static void setFlushNetworkPolicy(FlushNetworkPolicy types);

        /**
         * 首次设置用户属性
         * @param properties 用户属性
         */
        static void profileSetOnce(const ObjectNode &properties);

        /**
         * 删除事件计时
         * @param eventName 事件名称
         */
        static void removeTimer(const char *eventName);

        /**
         * 用于在 App 首次启动时追踪渠道来源
         * @param properties 激活事件的属性
         * @param disableCallback 是否关闭这次渠道匹配的回调请求
         */
        static void trackAppInstall(const ObjectNode &properties, bool disableCallback);

        /**
         * 用于在 App 首次启动时追踪渠道来源
         */
        static void trackAppInstall();

        /**
         * 删除本地缓存的事件与用户数据
         */
        static void deleteAll();

        /**
         * 设置 item
         * @param itemType item 类型
         * @param itemId item ID
         * @param properties item 相关属性
         */
        static void itemSet(const char *itemType, const char *itemId, const ObjectNode &properties);

        /**
         * 删除 item
         * @param itemType item 类型
         * @param itemId item ID
         */
        static void itemDelete(const char *itemType, const char *itemId);
    };
}

#endif // COCOS2DX_SENSORS_ANALYTICS_H_
