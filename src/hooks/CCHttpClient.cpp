#include <Geode/modify/CCHttpClient.hpp>
#include "../managers/RequestStutter.hpp"

using namespace geode::prelude;

static std::vector<CCHttpRequest*> handledReqs;

class $modify(NRLCCHttpClient, CCHttpClient) {
    static void onModify(auto& self) {
        (void) self.setHookPriority("CCHttpClient::send", -99999);
    }

    void send(CCHttpRequest* request) {
        if (const std::string url = request->getUrl(); url.find("://www.boomlings.com") != std::string::npos && std::find(handledReqs.begin(), handledReqs.end(), request) == handledReqs.end()) {
            auto downloadLevel = false;
            if (url.find("://www.boomlings.com/database/downloadGJLevel22.php") != std::string::npos) {
                downloadLevel = true;
            }
            handledReqs.push_back(request);
            if (const auto time = RequestStutter::getRequestTime(downloadLevel); time > 0) {
                request->retain();
                //CCScheduler::get()->scheduleSelector(
                //    schedule_selector(NRLCCHttpClient::sendRequest),
                //    this,
                //    1,
                //    0,
                //    time / 1000.0f,
                //    false
                //);
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

    //void sendRequest(float dt) {
    //    CCHttpClient::send(m_fields->m_request);
    //    m_fields->m_request->release();
    //}
};