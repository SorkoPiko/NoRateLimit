#include <Geode/utils/web.hpp>
#include "../managers/RequestStutter.hpp"

using namespace geode::prelude;

web::WebTask WebRequest_send(web::WebRequest* request, const std::string_view method, const std::string_view url) {
    if (url.find("boomlings.com") != std::string::npos) {
        if (const auto time = RequestStutter::getRequestTime(); time > 0) {
            const auto req = new web::WebRequest(*request);
            const auto returnTask = web::WebTask::run([method, url, req, time](auto progress, auto cancelled) -> web::WebTask::Result {
                std::this_thread::sleep_for(std::chrono::milliseconds(time));

                web::WebTask task = req->send(method, url);

                task.listen([progress, cancelled](const web::WebResponse* taskResponse) -> web::WebTask::Result {
                    if (cancelled()) {
                        delete taskResponse;
                        return web::WebTask::Cancel();
                    }
                    return *taskResponse;
                }, [progress, cancelled](web::WebProgress* taskProgress) {
                    if (!cancelled()) progress(*taskProgress);
                });

                while (!cancelled()) std::this_thread::sleep_for(std::chrono::milliseconds(2));

                task.cancel();
                return web::WebTask::Cancel();
            }, fmt::format("{} {}", method, url));
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