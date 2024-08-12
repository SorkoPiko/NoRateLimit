#include <Geode/modify/CCHttpClient.hpp>
#include "../managers/RequestStutter.hpp"

using namespace geode::prelude;

class $modify(NRLCCHttpClient, CCHttpClient) {
    void send(CCHttpRequest* request) {
        if (const std::string url = request->getUrl(); url.find("boomlings.com") != std::string::npos) {
            if (const auto time = RequestStutter::getRequestTime(); time > 0) {
                std::thread().detach();
                std::this_thread::sleep_for(std::chrono::milliseconds(time));
            }
        }
        CCHttpClient::send(request);
    }
};