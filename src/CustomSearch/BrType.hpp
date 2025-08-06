#pragma once

#include <vector>
#include <string>

using namespace geode::prelude;

class BrType {
    public:
        inline static std::vector<std::tuple<int, int,matjson::Value>> LevelID;
        inline static bool LoadedAllLevels = false;
        inline static int filterType;
        inline static bool isSearchingBR;
        inline static bool MapPack_Br = false;
        inline static bool shownServerError = false;
        inline static gd::string ShouldChangeText;
        static std::optional<std::tuple<int, int, matjson::Value>> find(int id) {
            auto it = std::find_if(LevelID.begin(), LevelID.end(),
            [id](const std::tuple<int, int, matjson::Value>& tuple) {
                return std::get<1>(tuple) == id; // Compare the second element
            });

            if (it != LevelID.end()) {
                return *it; 
            }

            return std::nullopt;
        }


       inline static void parseRequestString(const std::map<int, matjson::Value> level_map) {
            LevelID.clear();
            for (const auto& [key, value] : level_map) {
                if (value.contains("id") && value["id"].isInt()) {
                    int ids = value["id"].asInt().unwrap();
                    if (ids < 0) {
                        ids*=-1;
                    }
                    LevelID.emplace_back(key, ids,value);
                }
            }
            std::sort(LevelID.begin(), LevelID.end(), 
                    [](const std::tuple<int, int, matjson::Value>& a, const std::tuple<int, int, matjson::Value>& b) {
                        return std::get<0>(a) < std::get<0>(b);
                    });
            LoadedAllLevels = true;
        }

        inline static GJSearchObject* getSearchObject(int upper, int lower, bool map_Pack) {
            std::stringstream download;
            if (map_Pack) {
                download << "MapPack"_spr;
                return GJSearchObject::create(SearchType::MapPack, download.str());
            }
            bool first = true;
            // log::debug("Dowload Upper: {}, Download Lower {}", upper,lower );
            if (!(upper == 0 && lower == 0)) {
                if (upper > lower) {
                    for (unsigned int i = lower; i < upper; i++) {
                        if (!first) {
                            download << ",";
                        }
                        if (i < BrType::LevelID.size()) { 
                            download << std::to_string(std::get<1>(BrType::LevelID.at(i)));
                            first = false;
                        } 
                    }
                }
            } else {
                if (!BrType::LevelID.empty()) {
                    download << std::to_string(std::get<1>(BrType::LevelID.at(0)));
                }
            }
            download << "&gameVersion=22";
            GJSearchObject* searchObj = GJSearchObject::create(SearchType::Type19, download.str());
            return searchObj;
        }   

        
};