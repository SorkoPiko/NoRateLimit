#include <Geode/modify/CCHttpClient.hpp>
#include "../managers/RequestStutter.hpp"

using namespace geode::prelude;

static std::vector<CCHttpRequest*> handledReqs;

class $modify(NRLCCHttpClient, CCHttpClient) {
    static void onModify(auto& self) {
        (void) self.setHookPriority("CCHttpClient::send", -99999);
    }

    void send(CCHttpRequest* request) {
        log::info("intercept");
        if (const std::string url = request->getUrl(); url.find("boomlings.com") != std::string::npos && std::find(handledReqs.begin(), handledReqs.end(), request) == handledReqs.end()) {
            handledReqs.push_back(request);
            if (const auto time = RequestStutter::getRequestTime(); time > 0) {
                log::info("delaying request by {}ms", time);
                request->retain();
                std::thread([this, request, time] {
                    std::this_thread::sleep_for(std::chrono::milliseconds(time));
                    Loader::get()->queueInMainThread([this, request] {
                        CCHttpClient::send(request);
                        request->release();
                    });
                }).detach();
                return;
            }
        }
        CCHttpClient::send(request);
    }
};