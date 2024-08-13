#include <Geode/modify/CCHttpClient.hpp>
#include "../managers/RequestStutter.hpp"

using namespace geode::prelude;

class $modify(NRLCCHttpClient, CCHttpClient) {
    static void onModify(auto& self) {
        (void) self.setHookPriority("CCHttpClient::send", -99999);
    }

    void send(CCHttpRequest* request) {
        if (const std::string url = request->getUrl(); url.find("boomlings.com") != std::string::npos) {
            if (const auto time = RequestStutter::getRequestTime(); time > 0) {
                std::thread([this, request, time] {
                    std::this_thread::sleep_for(std::chrono::milliseconds(time));
                    log::info("executed request after {}ms", time);
                    Loader::get()->queueInMainThread([this, request] {CCHttpClient::send(request);});
                }).detach();
                return;
            }
        }
        CCHttpClient::send(request);
    }
};