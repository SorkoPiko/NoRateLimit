#include <Geode/utils/web.hpp>
#include <future>
#include <Geode/loader/Mod.hpp>
#include <Geode/utils/addresser.hpp>
#include "../managers/RequestStutter.hpp"

using namespace geode::prelude;

static std::vector<size_t> handledReqs;

web::WebTask WebRequest_send(web::WebRequest* request, const std::string_view method, const std::string_view url) {
    if (url.find("://www.boomlings.com") != std::string::npos) {
        auto downloadLevel = false;
        if (url.find("://www.boomlings.com/database/downloadGJLevel22.php") != std::string::npos) {
            downloadLevel = true;
        }
        if (const auto time = RequestStutter::getRequestTime(downloadLevel); time > 0 && std::find(handledReqs.begin(), handledReqs.end(), request->getID()) == handledReqs.end()) {
            handledReqs.push_back(request->getID());

            const auto req = new web::WebRequest(*request);
            const auto returnTask = web::WebTask::run([method, url, req, time](auto progress, auto cancelled) -> web::WebTask::Result {
                std::this_thread::sleep_for(std::chrono::milliseconds(time));

                const web::WebResponse* response = nullptr;
                auto task = req->send(method, url);

                task.listen([&response](const auto taskResponse) {
                    response = taskResponse;
                }, [progress, cancelled](auto taskProgress) {
                    if (!cancelled()) {
                        progress(*taskProgress);
                    }
                });

                while (!response && !cancelled()) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                }

                if (cancelled()) {
                    task.cancel();
                    delete req;
                    return web::WebTask::Cancel();
                } else {
                    auto result = *response;
                    delete req;
                    return result;
                }

            }, fmt::format("NRL {} {}", method, url));
            return returnTask;
        }
    }
    return request->send(method, url);
}

$execute {
    auto hook = Mod::get()->hook(
        reinterpret_cast<void*>(getNonVirtual(&web::WebRequest::send)),
        &WebRequest_send,
        "geode::web::WebRequest::send",
        tulip::hook::TulipConvention::Thiscall
    );
    if (hook.err()) {
        log::error("{}", hook.unwrapErr());
        return;
    }
    hook.unwrap()->setPriority(-99999);
}