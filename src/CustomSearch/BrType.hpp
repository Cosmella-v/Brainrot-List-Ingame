#ifndef SearchTypeHpp
#define SearchTypeHpp
#include <vector>
#include <string>

using namespace geode::prelude;

class BrType {
    public:
        inline static std::vector<std::pair<int, int>> LevelID;
        inline static bool LoadedAllLevels = false;
        inline static int filterType;
        inline static bool isSearchingBR;
        static int find(int id) {
            auto it = std::find_if(LevelID.begin(), LevelID.end(),
                [id](const std::pair<int, int>& pair) {
                    return pair.second == id;
                });

            if (it != LevelID.end()) {
                return it->first; 
            }

            return -1;
        }


       inline static void parseRequestString(const std::map<int, matjson::Value> level_map) {
            LevelID.clear();
            for (const auto& [key, value] : level_map) {
                if (value.contains("id")) {
                    int ids = value["id"].asInt().unwrap();
                    if (ids < 0) {
                        ids*=-1;
                    }
                    LevelID.emplace_back(key, ids);
                }
            }
            std::sort(LevelID.begin(), LevelID.end(), 
                    [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                        return a.first < b.first;
                    });
            LoadedAllLevels = true;
        }

        inline static GJSearchObject* getSearchObject(int upper, int lower) {
            std::stringstream download;
            bool first = true;
            // log::debug("Dowload Upper: {}, Download Lower {}", upper,lower );
            if (!(upper == 0 && lower == 0)) {
                if (upper > lower) {
                    for (unsigned int i = lower; i < upper; i++) {
                        if (!first) {
                            download << ",";
                        }
                        if (i < BrType::LevelID.size()) { 
                            download << std::to_string(BrType::LevelID.at(i).second);
                            first = false;
                        } 
                    }
                }
            } else {
                if (!BrType::LevelID.empty()) {
                    download << std::to_string(BrType::LevelID.at(0).second);
                }
            }
            download << "&gameVersion=22";
            GJSearchObject* searchObj = GJSearchObject::create(SearchType::Type19, download.str());
            return searchObj;
        }   

        
};

#endif