#include <Geode/loader/Mod.hpp>
#include <chrono>

#include "RequestStutter.hpp"

using namespace geode::prelude;

long long RequestStutter::lastRequestTime = 0;
long long RequestStutter::lastDownloadLevelTime = 0;

long long getCurrentTimestampMillis() {
    const auto now = std::chrono::system_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return duration.count();
}

long long RequestStutter::getRequestTime(bool downloadLevel) {
    const auto maxReqs = Mod::get()->getSettingValue<int64_t>("maxReqs");
    const auto delay = 60 * 1000 / maxReqs;
    const auto now = getCurrentTimestampMillis();
    const auto nextRequestTime = lastRequestTime + delay;
    lastRequestTime = std::max(nextRequestTime, now);
    
    if (downloadLevel) {
        return getDownloadLevelTime();
    }
    
    return std::max(0LL, lastRequestTime - now);
}

long long RequestStutter::getDownloadLevelTime() {
    const auto maxReqs = Mod::get()->getSettingValue<int64_t>("maxDownloadLevelReqs");
    const auto delay = 60 * 1000 / maxReqs;
    const auto now = getCurrentTimestampMillis();
    const auto nextDownloadLevelTime = lastDownloadLevelTime + delay;
    lastDownloadLevelTime = std::max(nextDownloadLevelTime, now);
    
    if (lastDownloadLevelTime < lastRequestTime) {
        lastDownloadLevelTime = lastRequestTime;
    }
    
    return std::max(0LL, lastDownloadLevelTime - now);
}
