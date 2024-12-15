#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <thread>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>
using namespace geode::prelude;

static std::map<int,matjson::Value> level_map;

void RequestApi(bool retry,std::string Url, std::function<void(matjson::Value)> fun) {
      web::WebRequest().get(Url).listen(
        [=](auto getval) {
            auto json = getval->json().unwrapOr("failed");
            if (json == "failed") {
                log::error("Failed at {}",Url);
                if (retry) {
                     RequestApi(retry,Url,fun);
                } 
               return;
            }

            try {
                fun(json); 
            } catch (const std::exception& ex) {
                 log::error("Error doing function", ex.what());
            }
        },
        [](auto prog) {
            
        },
        []() {
            log::warn("Request was cancelled.");
        }
    );
}

static void getlistjson(std::function<void(matjson::Value)> fun) {
   log::debug("should send https://br-list.pages.dev/data/_list.json");
    RequestApi(false,"https://br-list.pages.dev/data/_list.json", fun);
}

static void getpackjson(std::function<void(matjson::Value)> fun) {
    RequestApi(false,"https://br-list.pages.dev/data/_packlist.json", fun);
}

static void getleveljson(const std::string& name, std::function<void(matjson::Value)> fun) {
    std::string finalurl;
    for (char c : name) {
        if (c == ' ') {
            finalurl += "%20"; 
        } else {
            finalurl += c; 
        }
    }
    log::debug("getting info for {}",name);
    RequestApi(true,"https://br-list.pages.dev/data/" + finalurl + ".json", fun);
}
