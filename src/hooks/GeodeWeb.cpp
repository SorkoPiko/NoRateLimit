#include <Geode/utils/web.hpp>
#include <future>
#include <Geode/loader/Mod.hpp>
#include <Geode/utils/addresser.hpp>
#include "../managers/RequestStutter.hpp"

using namespace geode::prelude;

static std::vector<size_t> handledReqs;

// Thanks SMJS for helping me get this to work
web::WebTask WebRequest_send(web::WebRequest* request, const std::string_view methodView, const std::string_view urlView) {
    const size_t id = request->getID();
    const std::string method(methodView);
    const std::string url(urlView);

    if (url.find("://www.boomlings.com") != std::string::npos) {
        auto downloadLevel = false;
        if (url.find("://www.boomlings.com/database/downloadGJLevel22.php") != std::string::npos) {
            downloadLevel = true;
        }
        if (const auto time = RequestStutter::getRequestTime(downloadLevel); time > 0 && std::find(handledReqs.begin(), handledReqs.end(), id) == handledReqs.end()) {
            handledReqs.push_back(id);

            const auto req = new web::WebRequest(*request);

            return web::WebTask::run([method, url, req, time](auto progress, auto cancelled) -> web::WebTask::Result {
                std::unique_ptr<web::WebResponse> response;

                std::this_thread::sleep_for(std::chrono::milliseconds(time));

                web::WebTask task = req->send(method, url);
                task.listen([&response](const web::WebResponse* taskResponse) {
                    response = std::make_unique<web::WebResponse>(*taskResponse);
                }, [progress, cancelled](web::WebProgress* taskProgress) {
                    if (!cancelled()) progress(*taskProgress);
                });

                while (!response && !cancelled()) std::this_thread::sleep_for(std::chrono::milliseconds(2));

                if (cancelled()) {
                    task.cancel();

                    return web::WebTask::Cancel();
                }
                return *response;

            }, fmt::format("NRL {} {}", method, url));
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
