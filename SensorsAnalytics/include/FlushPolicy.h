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

#ifndef COCOS2DX_SENSORS_FLUSH_NETWORK_POLICY_H_
#define COCOS2DX_SENSORS_FLUSH_NETWORK_POLICY_H_

namespace sensorsdata {
    enum FlushNetworkPolicy {
        kFlushNone = 0,
        kFlush2G = 1 << 0,
        kFlush3G = 1 << 1,
        kFlush4G = 1 << 2,
        kFlushWiFi = 1 << 3,
        kFlush5G = 1 << 4,
        kFlushAll = 0xFF,
    };
}

namespace sensorsdata {
    template<class T> inline T operator | (T a, T b) { return (T)((int)a | (int)b); }
}

#endif // COCOS2DX_SENSORS_FLUSH_NETWORK_POLICY_H_
