#pragma once

#include <Geode/Geode.hpp>
#include <Geode/Bindings.hpp>
#include <vector>
#include <string>

inline int offset = 5000;

using namespace geode::prelude;

class BRPacks {
public:
    inline static std::map<std::string,matjson::Value> level_map;
    inline static std::map<int,matjson::Value> levelid_map;
    inline static std::vector<std::tuple<std::string,matjson::Value>> PacksIDs;
    inline static int maxCount = 0;
    inline static int realMaxCount = 0;

    inline static cocos2d::CCArray* getMapPacks(int page) {
        cocos2d::CCArray* Newarrow = new cocos2d::CCArray;
        int index = offset;
        int realcount = 0;
        for (auto data : PacksIDs) {
            realcount += 1;
            if (realcount < (page * 10)) {
                continue;
            }
            index += 1;
            //log::debug("{} . {} . {}",std::get<0>(data),std::get<1>(data),std::get<2>(data).dump());
            auto pack = BRPacks::createBRPack(index + (page * 10), data);
            if (pack) {
                Newarrow->addObjectNew(pack);
            }
            if (realcount >= ( (page + 1) * 10)) {
                break;
            }

        }
        BRPacks::maxCount = ceil(PacksIDs.size() / 10);
        BRPacks::realMaxCount = PacksIDs.size();
        log::debug("BRPacks maxCount: {}", BRPacks::maxCount);
        return Newarrow;
    }
    
    static std::optional<std::tuple<int, int, matjson::Value>> find(int id) {
        auto letme = levelid_map.find(id);
        if (letme != levelid_map.end()) {
            return std::make_optional(std::make_tuple(200, letme->second["id"].asInt().unwrap(), letme->second));
        } else {
            return std::nullopt; 
        }
    }

private:
    static GJMapPack* createBRPack(int index, std::tuple<std::string,matjson::Value> packet) {
        log::info("attempting to create pack: {} index, {} foobar, {} levels", index, std::get<0>(packet), std::get<1>(packet));
        if (!std::get<1>(packet).isArray()) return nullptr;
        auto levels = std::get<1>(packet).asArray().unwrap();
        if (levels.empty()) return nullptr;
        CCDictionary* packData = CCDictionary::create();
        GJMapPack* pack;
        //log::debug("{}",std::get<0>(packet));
        packData->setObject(CCString::create(std::to_string(index)), "1");
        packData->setObject(CCString::create(std::get<0>(packet)), "2");
        std::stringstream download;
        bool first = true;
        for (auto level : levels) {
            if (!first) download << ",";
            download << std::to_string(BRPacks::level_map.at(level.asString().unwrap())["id"].asInt().unwrap());
            first = false;
        }
        packData->setObject(CCString::create(download.str()), "3");

        packData->setObject(CCString::create("0"), "4");
        packData->setObject(CCString::create("0"), "5");
        packData->setObject(CCString::create("6"), "6");
        packData->setObject(CCString::create("255,255,255"), "7");
        packData->setObject(CCString::create("255,255,255"), "8");
        pack = GJMapPack::create(packData);
        pack->setUserObject("modified-by-brl"_spr, CCBool::create(true));
        return pack;
    }
};