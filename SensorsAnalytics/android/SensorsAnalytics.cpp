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

#include "../include/SensorsAnalytics.h"
#include "cocos2d.h"

#define SENSORS_ANALYTICS_JAVA_CLASS "com/sensorsdata/analytics/android/sdk/SensorsDataAPI"

using namespace sensorsdata;
USING_NS_CC;

extern "C" {

// 是否在事件属性中增加 $lib_plugin_version 属性
static bool isAddVersion = true;
// SDK 全局实例
static jobject sSensorsAPI = NULL;
// Jni 全局实例
static JniMethodInfo sInfo;

/**
 *  获取 SDK 实例
 * @return 返回 SDK 实例
 */
static jobject getSDKInstance() {
    if (sSensorsAPI != NULL) {
        return sSensorsAPI;
    }

    if (JniHelper::getStaticMethodInfo(sInfo,
                                       SENSORS_ANALYTICS_JAVA_CLASS,
                                       "sharedInstance",
                                       "()Lcom/sensorsdata/analytics/android/sdk/SensorsDataAPI;")) {
        jobject sensorsAPI = sInfo.env->CallStaticObjectMethod(sInfo.classID, sInfo.methodID);
        //  建立全局引用，在多线程环境下使用
        sSensorsAPI = (jobject) sInfo.env->NewGlobalRef(sensorsAPI);
        sInfo.env->DeleteLocalRef(sensorsAPI);
    }
    return sSensorsAPI;
}

/**
 * ObjectNode 转化为 JSONObjec 对象
 * @param env env
 * @param properties ObjectNode
 * @return 返回 JSONObject 对象
 */
jobject createJavaJsonObject(JNIEnv *env, const sensorsdata::ObjectNode *properties) {
    jclass classJSONObject = env->FindClass("org/json/JSONObject");
    jmethodID constructMethod = env->GetMethodID(classJSONObject,
                                                 "<init>",
                                                 "(Ljava/lang/String;)V");
    string js = ObjectNode::toJson(*properties);
    jobject objJSON = env->NewObject(classJSONObject, constructMethod,
                                     env->NewStringUTF(js.c_str()));
    env->DeleteLocalRef(classJSONObject);
    return objJSON;
}

/**
 * 将 jstring 转化为 string
 * @param env env
 * @param jString jString
 * @return
 */
static string jStringToString(JNIEnv *env, jstring jString) {
    if (jString == NULL || env == NULL) {
        return "";
    }
    const char *chars = env->GetStringUTFChars(jString, NULL);
    string ret(chars);
    env->ReleaseStringUTFChars(jString, chars);
    return ret;
}

/**
 * 判断 SDK 是否存在方法
 * @param methodName 方法名
 * @param paramCode 方法签名
 * @return 是否存在方法，SDK 对象存在且存在方法时，返回 true，其它情况返回 false
 */
static bool isSDKMethodExist(const char *methodName, const char *paramCode) {
    if (getSDKInstance() != NULL &&
        JniHelper::getMethodInfo(sInfo,
                                 SENSORS_ANALYTICS_JAVA_CLASS,
                                 methodName,
                                 paramCode)) {
        return true;
    }
    return false;
}

/**
 * 添加 $lib_plugin_version 属性
 * @param properties 事件属性
 */
static void appendLibPluginVersion(ObjectNode &properties) {
    std::map<string, ObjectNode::ValueNode>::iterator time_property_iter =
            properties.propertiesMap.find(SENSORS_ANALYTICS_PLUGIN_VERSION_KEY);
    if (isAddVersion && time_property_iter == properties.propertiesMap.end()) {
        std::vector<std::string> libVersion;
        libVersion.push_back(SENSORS_ANALYTICS_PLUGIN_VERSION_VALUE);
        properties.setList(SENSORS_ANALYTICS_PLUGIN_VERSION_KEY, libVersion);
        isAddVersion = false;
    }
}
}

void SensorsAnalytics::track(const char *eventName, const ObjectNode &properties) {
    // 判断是否存在方法
    if (isSDKMethodExist("track", "(Ljava/lang/String;Lorg/json/JSONObject;)V")) {
        jstring jEventName = sInfo.env->NewStringUTF(eventName);
        ObjectNode recordProperties;
        // 复制 properties 到 recordProperties
        recordProperties.mergeFrom(properties);
        // 添加 $lib_plugin_version 属性
        appendLibPluginVersion(recordProperties);
        // 创建 JSONObject 对象
        jobject jParam = createJavaJsonObject(sInfo.env, &recordProperties);
        // 调用 track 方法
        sInfo.env->CallVoidMethod(getSDKInstance(), sInfo.methodID, jEventName, jParam);
        // 释放对象
        sInfo.env->DeleteLocalRef(jParam);
        sInfo.env->DeleteLocalRef(jEventName);
    }
}

void SensorsAnalytics::track(const char *eventName) {
    ObjectNode objectNode;
    // 直接调用 track ，方便添加 $lib_plugin_version 属性
    track(eventName, objectNode);
}

void SensorsAnalytics::setFlushNetworkPolicy(FlushNetworkPolicy types) {
    int t = static_cast<int>(types);
    if (isSDKMethodExist("setFlushNetworkPolicy", "(I)V")) {
        sInfo.env->CallVoidMethod(getSDKInstance(), sInfo.methodID, t);
    }
}

string SensorsAnalytics::getSuperProperties() {
    string superProperties;
    if (isSDKMethodExist("getSuperProperties", "()Lorg/json/JSONObject;")) {
        jobject jsonObject = sInfo.env->CallObjectMethod(getSDKInstance(), sInfo.methodID);
        // 调用 JSONObject 的 toString 方法，返回字符串
        if (jsonObject != NULL &&
            JniHelper::getMethodInfo(sInfo, "org/json/JSONObject",
                                     "toString", "()Ljava/lang/String;")) {
            jstring jsonString = (jstring) sInfo.env->CallObjectMethod(jsonObject, sInfo.methodID);
            superProperties = jStringToString(sInfo.env, jsonString);
        }
    }
    if (superProperties.empty()) {
        return "{}";
    }
    return superProperties;
}

void SensorsAnalytics::identify(const char *anonymousId) {
    if (isSDKMethodExist("identify", "(Ljava/lang/String;)V")) {
        jstring jAnonymousId = sInfo.env->NewStringUTF(anonymousId);
        sInfo.env->CallVoidMethod(getSDKInstance(), sInfo.methodID, jAnonymousId);
        sInfo.env->DeleteLocalRef(jAnonymousId);
    }
}

void SensorsAnalytics::login(const char *loginId) {
    if (isSDKMethodExist("login", "(Ljava/lang/String;Lorg/json/JSONObject;)V")) {
        jstring jLoginId = sInfo.env->NewStringUTF(loginId);
        ObjectNode recordProperties;
        appendLibPluginVersion(recordProperties);
        // 创建 JSONObject 对象
        jobject jParam = createJavaJsonObject(sInfo.env, &recordProperties);
        sInfo.env->CallVoidMethod(getSDKInstance(), sInfo.methodID, jLoginId, jParam);
        sInfo.env->DeleteLocalRef(jLoginId);
    }
}

void SensorsAnalytics::logout() {
    if (isSDKMethodExist("logout", "()V")) {
        sInfo.env->CallVoidMethod(getSDKInstance(), sInfo.methodID);
    }
}

void SensorsAnalytics::profileSet(const ObjectNode &properties) {
    if (isSDKMethodExist("profileSet", "(Lorg/json/JSONObject;)V")) {
        jobject jParam = createJavaJsonObject(sInfo.env, &properties);
        sInfo.env->CallVoidMethod(getSDKInstance(), sInfo.methodID, jParam);
        sInfo.env->DeleteLocalRef(jParam);
    }
}

void SensorsAnalytics::flush() {
    if (isSDKMethodExist("flush", "()V")) {
        sInfo.env->CallVoidMethod(getSDKInstance(), sInfo.methodID);
    }
}

string SensorsAnalytics::trackTimerStart(const char *eventName) {
    string eventNameRegex;
    if (isSDKMethodExist("trackTimerStart", "(Ljava/lang/String;)Ljava/lang/String;")) {
        jstring jEventName = sInfo.env->NewStringUTF(eventName);
        jstring jEventNameRegex = (jstring) sInfo.env->CallObjectMethod(getSDKInstance(),
                                                                        sInfo.methodID, jEventName);
        eventNameRegex = jStringToString(sInfo.env, jEventNameRegex);
        sInfo.env->DeleteLocalRef(jEventName);
    }
    return eventNameRegex;
}

void SensorsAnalytics::trackTimerPause(const char *eventName) {
    if (isSDKMethodExist("trackTimerPause", "(Ljava/lang/String;)V")) {
        jstring jEventName = sInfo.env->NewStringUTF(eventName);
        sInfo.env->CallVoidMethod(getSDKInstance(), sInfo.methodID, jEventName);
    }
}

void SensorsAnalytics::trackTimerResume(const char *eventName) {
    if (isSDKMethodExist("trackTimerResume", "(Ljava/lang/String;)V")) {
        jstring jEventName = sInfo.env->NewStringUTF(eventName);
        sInfo.env->CallVoidMethod(getSDKInstance(), sInfo.methodID, jEventName);
        sInfo.env->DeleteLocalRef(jEventName);
    }
}

void SensorsAnalytics::trackTimerEnd(const char *eventName, const ObjectNode &properties) {
    if (isSDKMethodExist("trackTimerEnd", "(Ljava/lang/String;Lorg/json/JSONObject;)V")) {
        jstring jEventName = sInfo.env->NewStringUTF(eventName);
        ObjectNode recordProperties;
        // 复制 properties 到 recordProperties
        recordProperties.mergeFrom(properties);
        // 添加 $lib_plugin_version 属性
        appendLibPluginVersion(recordProperties);
        jobject jParam = createJavaJsonObject(sInfo.env, &recordProperties);
        sInfo.env->CallVoidMethod(getSDKInstance(), sInfo.methodID, jEventName, jParam);
        sInfo.env->DeleteLocalRef(jEventName);
        sInfo.env->DeleteLocalRef(jParam);
    }
}

void SensorsAnalytics::clearTrackTimer() {
    if (isSDKMethodExist("clearTrackTimer", "()V")) {
        sInfo.env->CallVoidMethod(getSDKInstance(), sInfo.methodID);
    }
}

void SensorsAnalytics::removeTimer(const char *eventName) {
    if (isSDKMethodExist("removeTimer", "(Ljava/lang/String;)V")) {
        jstring jEventName = sInfo.env->NewStringUTF(eventName);
        sInfo.env->CallVoidMethod(getSDKInstance(), sInfo.methodID, jEventName);
        sInfo.env->DeleteLocalRef(jEventName);
    }
}

void SensorsAnalytics::registerSuperProperties(const ObjectNode &properties) {
    if (isSDKMethodExist("registerSuperProperties", "(Lorg/json/JSONObject;)V")) {
        jobject jParam = createJavaJsonObject(sInfo.env, &properties);
        sInfo.env->CallVoidMethod(getSDKInstance(), sInfo.methodID, jParam);
        sInfo.env->DeleteLocalRef(jParam);
    }
}

void SensorsAnalytics::unregisterSuperProperty(const char *superPropertyName) {
    if (isSDKMethodExist("unregisterSuperProperty", "(Ljava/lang/String;)V")) {
        jstring jSuperPropertyName = sInfo.env->NewStringUTF(superPropertyName);
        sInfo.env->CallVoidMethod(getSDKInstance(), sInfo.methodID, jSuperPropertyName);
        sInfo.env->DeleteLocalRef(jSuperPropertyName);
    }
}

void SensorsAnalytics::clearSuperProperties() {
    if (isSDKMethodExist("clearSuperProperties", "()V")) {
        sInfo.env->CallVoidMethod(getSDKInstance(), sInfo.methodID);
    }
}

void SensorsAnalytics::profileSetOnce(const ObjectNode &properties) {
    if (isSDKMethodExist("profileSetOnce", "(Lorg/json/JSONObject;)V")) {
        jobject jParam = createJavaJsonObject(sInfo.env, &properties);
        sInfo.env->CallVoidMethod(getSDKInstance(), sInfo.methodID, jParam);
        sInfo.env->DeleteLocalRef(jParam);
    }
}

void SensorsAnalytics::trackAppInstall(const ObjectNode &properties, bool disableCallback) {
    if (isSDKMethodExist("trackInstallation", "(Ljava/lang/String;Lorg/json/JSONObject;Z)V")) {
        jobject jParam = createJavaJsonObject(sInfo.env, &properties);
        jstring jEventName = sInfo.env->NewStringUTF("$AppInstall");
        sInfo.env->CallVoidMethod(getSDKInstance(), sInfo.methodID, jEventName, jParam,
                                  disableCallback);
        sInfo.env->DeleteLocalRef(jEventName);
        sInfo.env->DeleteLocalRef(jParam);
    }
}

void SensorsAnalytics::trackAppInstall() {
    if (isSDKMethodExist("trackInstallation", "(Ljava/lang/String;)V")) {
        jstring jEventName = sInfo.env->NewStringUTF("$AppInstall");
        sInfo.env->CallVoidMethod(getSDKInstance(), sInfo.methodID, jEventName);
        sInfo.env->DeleteLocalRef(jEventName);
    }
}

void SensorsAnalytics::deleteAll() {
    if (isSDKMethodExist("deleteAll", "()V")) {
        sInfo.env->CallVoidMethod(getSDKInstance(), sInfo.methodID);
    }
}

void
SensorsAnalytics::itemSet(const char *itemType, const char *itemId, const ObjectNode &properties) {
    if (isSDKMethodExist("itemSet",
                         "(Ljava/lang/String;Ljava/lang/String;Lorg/json/JSONObject;)V")) {
        jstring jItemType = sInfo.env->NewStringUTF(itemType);
        jstring jItemId = sInfo.env->NewStringUTF(itemId);
        jobject jParam = createJavaJsonObject(sInfo.env, &properties);
        sInfo.env->CallVoidMethod(getSDKInstance(), sInfo.methodID, jItemType, jItemId, jParam);
        sInfo.env->DeleteLocalRef(jItemType);
        sInfo.env->DeleteLocalRef(jItemId);
        sInfo.env->DeleteLocalRef(jParam);
    }
}

void SensorsAnalytics::itemDelete(const char *itemType, const char *itemId) {
    if (isSDKMethodExist("itemDelete", "(Ljava/lang/String;Ljava/lang/String;)V")) {
        jstring jItemType = sInfo.env->NewStringUTF(itemType);
        jstring jItemId = sInfo.env->NewStringUTF(itemId);
        sInfo.env->CallVoidMethod(getSDKInstance(), sInfo.methodID, jItemType, jItemId);
        sInfo.env->DeleteLocalRef(jItemType);
        sInfo.env->DeleteLocalRef(jItemId);
    }
}

void SensorsAnalytics::trackTimerEnd(const char *eventName) {
    ObjectNode objectNode;
    // 直接调用 trackTimerEnd ，方便添加 $lib_plugin_version 属性
    trackTimerEnd(eventName, objectNode);
}