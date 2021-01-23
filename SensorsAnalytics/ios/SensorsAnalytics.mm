/*
 * Created by yuqiang on 20/12/27.
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

#if ! __has_feature(objc_arc)
#error This file must be compiled with ARC. Either turn on ARC for the project or use -fobjc-arc flag on this file.
#endif

#include "SensorsAnalytics.h"
#if __has_include(<SensorsAnalyticsSDK/SensorsAnalyticsSDK.h>)
#import <SensorsAnalyticsSDK/SensorsAnalyticsSDK.h>
#else
#import "SensorsAnalyticsSDK.h"
#endif

using namespace sensorsdata;

static NSString *NSStringFromCString(const char* cstr){
    return cstr ? [NSString stringWithUTF8String:cstr] : nil;
}

static char *CStringFromNSString(NSString *string) {
    return (char *)[string UTF8String];
}

static NSDictionary *NSDictionaryFromObjectNode(const ObjectNode &node) {
    string json = ObjectNode::toJson(node);
    NSString *string = NSStringFromCString(json.c_str());
    NSData *data = [string dataUsingEncoding:NSUTF8StringEncoding];
    
    if (!data) return nil;
    return [NSJSONSerialization JSONObjectWithData:data options:kNilOptions error:nil];
}

static char *CStringFromNSDictionary(NSDictionary *dic) {
    
    if (!dic) return nil;
    NSData *jsonData = [NSJSONSerialization dataWithJSONObject:dic options:NSJSONWritingPrettyPrinted error:nil];
    
    if (!jsonData) return nil;
    NSString *jsonStr = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
    return (char *)[jsonStr UTF8String];
}

/// 通过插件触发的事件, 添加 $lib_plugin_version 属性
/// 1. 在应用程序生命周期中, 第一次通过插件 track 事件时, 需要添加 $lib_plugin_version 属性, 后续事件无需添加该属性
/// 2. 当用户的属性中包含 $lib_plugin_version 时, 插件不进行覆盖
/// @param properties 事件属性
static NSDictionary *PropertiesByAddingLibPluginVersionFromProperties(NSDictionary *properties) {
    if (properties[NSStringFromCString(SENSORS_ANALYTICS_PLUGIN_VERSION_KEY)]) return properties;
    __block NSMutableDictionary *result;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        result = [NSMutableDictionary dictionaryWithDictionary:properties];
        result[NSStringFromCString(SENSORS_ANALYTICS_PLUGIN_VERSION_KEY)] = @[NSStringFromCString(SENSORS_ANALYTICS_PLUGIN_VERSION_VALUE)];
    });
    return result ? [result copy] : properties;
}

void SensorsAnalytics::identify(const char *anonymousId) {
    [SensorsAnalyticsSDK.sharedInstance identify:NSStringFromCString(anonymousId)];
}

void SensorsAnalytics::track(const char *eventName) {
    ObjectNode properties;
    SensorsAnalytics::track(eventName, properties);
}

void SensorsAnalytics::track(const char *eventName, const ObjectNode &properties) {
    NSDictionary *dic = NSDictionaryFromObjectNode(properties);
    [SensorsAnalyticsSDK.sharedInstance track:NSStringFromCString(eventName)
                               withProperties:PropertiesByAddingLibPluginVersionFromProperties(dic)];
}

void SensorsAnalytics::login(const char *loginId) {
    [SensorsAnalyticsSDK.sharedInstance login:NSStringFromCString(loginId)
                               withProperties:PropertiesByAddingLibPluginVersionFromProperties(nil)];
}

void SensorsAnalytics::logout() {
    [SensorsAnalyticsSDK.sharedInstance logout];
}

void SensorsAnalytics::profileSet(const ObjectNode &properties) {
    [SensorsAnalyticsSDK.sharedInstance set:NSDictionaryFromObjectNode(properties)];
}

string SensorsAnalytics::getSuperProperties() {
    NSDictionary *properties = SensorsAnalyticsSDK.sharedInstance.currentSuperProperties;
    return string(CStringFromNSDictionary(properties));
}

void SensorsAnalytics::flush() {
    [SensorsAnalyticsSDK.sharedInstance flush];
}
 
string SensorsAnalytics::trackTimerStart(const char *eventName) {
    NSString *eventId = [SensorsAnalyticsSDK.sharedInstance trackTimerStart:NSStringFromCString(eventName)];
    return string(CStringFromNSString(eventId));
}

void SensorsAnalytics::trackTimerPause(const char *eventName) {
    [SensorsAnalyticsSDK.sharedInstance trackTimerPause:NSStringFromCString(eventName)];
}

void SensorsAnalytics::trackTimerResume(const char *eventName) {
    [SensorsAnalyticsSDK.sharedInstance trackTimerResume:NSStringFromCString(eventName)];
}

void SensorsAnalytics::trackTimerEnd(const char *eventName) {
    ObjectNode properties;
    SensorsAnalytics::trackTimerEnd(eventName, properties);
}

void SensorsAnalytics::trackTimerEnd(const char *eventName, const ObjectNode &properties) {
    NSDictionary *dic = NSDictionaryFromObjectNode(properties);
    [SensorsAnalyticsSDK.sharedInstance trackTimerEnd:NSStringFromCString(eventName)
                                       withProperties:PropertiesByAddingLibPluginVersionFromProperties(dic)];
}

void SensorsAnalytics::clearTrackTimer() {
    [SensorsAnalyticsSDK.sharedInstance clearTrackTimer];
}

void SensorsAnalytics::registerSuperProperties(const ObjectNode &properties) {
    [SensorsAnalyticsSDK.sharedInstance registerSuperProperties:NSDictionaryFromObjectNode(properties)];
}

void SensorsAnalytics::unregisterSuperProperty(const char *superPropertyName) {
    [SensorsAnalyticsSDK.sharedInstance unregisterSuperProperty:NSStringFromCString(superPropertyName)];
}

void SensorsAnalytics::clearSuperProperties() {
    [SensorsAnalyticsSDK.sharedInstance clearSuperProperties];
}

void SensorsAnalytics::setFlushNetworkPolicy(FlushNetworkPolicy types) {
    NSInteger result = types;
#ifdef __IPHONE_14_1
    if (result & SensorsAnalyticsNetworkType5G) {
        result = result | SensorsAnalyticsNetworkType5G;
    }
#endif
    [SensorsAnalyticsSDK.sharedInstance setFlushNetworkPolicy:result];
}

void SensorsAnalytics::profileSetOnce(const ObjectNode &properties) {
    [SensorsAnalyticsSDK.sharedInstance setOnce:NSDictionaryFromObjectNode(properties)];
}

void SensorsAnalytics::removeTimer(const char *eventName) {
    [SensorsAnalyticsSDK.sharedInstance removeTimer:NSStringFromCString(eventName)];
}

void SensorsAnalytics::trackAppInstall(const ObjectNode &properties, bool disableCallback) {
    [SensorsAnalyticsSDK.sharedInstance trackInstallation:@"$AppInstall"
                                           withProperties:NSDictionaryFromObjectNode(properties)
                                          disableCallback:disableCallback];
}

void SensorsAnalytics::trackAppInstall() {
    [SensorsAnalyticsSDK.sharedInstance trackInstallation:@"$AppInstall"];
}

void SensorsAnalytics::deleteAll() {
    [SensorsAnalyticsSDK.sharedInstance deleteAll];
}

void SensorsAnalytics::itemSet(const char *itemType, const char *itemId, const ObjectNode &properties) {
    [SensorsAnalyticsSDK.sharedInstance itemSetWithType:NSStringFromCString(itemType)
                                                 itemId:NSStringFromCString(itemId)
                                             properties:NSDictionaryFromObjectNode(properties)];
}

void SensorsAnalytics::itemDelete(const char *itemType, const char *itemId) {
    [SensorsAnalyticsSDK.sharedInstance itemDeleteWithType:NSStringFromCString(itemType)
                                                    itemId:NSStringFromCString(itemId)];
}
