#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <thread>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>
using namespace geode::prelude;

static std::map<int,matjson::Value> level_map;

inline void RequestApi(bool retry,std::string Url, std::function<void(matjson::Value)> fun,std::function<void()> onfail) {
      web::WebRequest().get(Url).listen(
        [=](auto getval) {
            auto json = getval->json().unwrapOr("failed");
            if (json == "failed") {
                //log::error("Failed at {}",Url);
                if (retry) {
                     RequestApi(retry,Url,fun,onfail);
                } else {
                    if (!onfail) {
                        return;
                    } else {
                        try {
                            onfail();
                        } catch (const std::exception& ex) {
                        log::error("Error doing function", ex.what());
                    }
                    }
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
        [=]() {
             if (retry) {
                     RequestApi(retry,Url,fun,onfail);
                } else {
                    if (!onfail) {
                        return;
                    } else {
                        try {
                            onfail();
                        } catch (const std::exception& ex) {
                        log::error("Error doing function", ex.what());
                    }
                    }
                }
            //log::warn("Request was cancelled.");
        }
    );
}

inline static void getlistjson(std::function<void(matjson::Value)> fun,std::function<void()> failed) {
   //log::debug("should send https://br-list.pages.dev/data/_list.json");
    RequestApi(false,"https://br-list.pages.dev/data/_list.json", fun,failed);
}

inline static void getpackjson(std::function<void(matjson::Value)> fun,std::function<void()> failed) {
    RequestApi(false,"https://br-list.pages.dev/data/_packlist.json", fun,failed);
}

inline static void getleveljson(const std::string& name, std::function<void(matjson::Value)> fun) {
    std::string finalurl;
    for (char c : name) {
        if (c == ' ') {
            finalurl += "%20"; 
        } else {
            finalurl += c; 
        }
    }
    //log::debug("getting info for {}",name);
    RequestApi(true,"https://br-list.pages.dev/data/" + finalurl + ".json", fun,nullptr);
}
