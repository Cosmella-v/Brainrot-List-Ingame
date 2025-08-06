#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <thread>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>
using namespace geode::prelude;
#include <iostream>
#include <string>

class WebviewUrl {
    public:
    inline static std::string ConvertToEmbed(std::string url) {
        return url;
    }
    inline static void Open(std::string url, std::string Name) {
        geode::utils::web::openLinkInBrowser(url);
    };
};


static std::map<int,matjson::Value> level_map;

inline void makeGeodeWebRequest(bool retry, std::string url, std::function<void(matjson::Value)> processFunction, std::function<void()> onFail) {
      web::WebRequest().get(url).listen(
        [=](auto getVal) {
            auto jsonUnwrapped = getVal->json().unwrapOr("");
            if (!jsonUnwrapped.isOk()) { log::error("Failed at {}", url); return onFail(); }
            processFunction(jsonUnwrapped); 
        }, [](auto prog) {
            // in progress
        }, [=]() {
            log::warn("request was cancelled");
            return onFail() // handle it as a fail
        }
    );
}

inline static void getBRListJSON(std::function<void(matjson::Value)> fun, std::function<void()> failed) {
   //log::debug("should send https://br-list.pages.dev/data/_list.json");
    makeGeodeWebRequest(false, "https://br-list.pages.dev/data/_list.json", fun, failed);
}

inline static void getBRPackJSON(std::function<void(matjson::Value)> fun, std::function<void()> failed) {
    makeGeodeWebRequest(false, "https://br-list.pages.dev/data/_packlist.json", fun, failed);
}

inline static void getBRLevelJSON(const std::string& name, std::function<void(matjson::Value)> fun) {
    std::string finalURL;
    for (char c : name) {
        if (c == ' ') {
            finalURL += "%20"; 
        } else {
            finalURL += c; 
        }
    }
    //log::debug("getting info for {}",name);
    makeGeodeWebRequest(true, fmt::format("https://br-list.pages.dev/data/{}.json", finalURL), fun, nullptr);
}
