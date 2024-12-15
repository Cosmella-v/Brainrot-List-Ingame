#ifndef SearchTypeHpp
#define SearchTypeHpp
#include <vector>
#include <string>

using namespace geode::prelude;

class BrType {
    public:
        inline static std::vector<int> LevelID;
        inline static bool firstTimeOpen = true;
        inline static int filterType;
        inline static bool isSearchingBR;

        inline static void parseRequestString(std::map<int,matjson::Value>  str) {
              for (const auto& pair : level_map) {
                int key = pair.first;
                matjson::Value value = pair.second;
                if (value.contains("id")) {
                    LevelID.push_back(value["id"].asInt().unwrap());
                }
            }
        }

        inline static GJSearchObject* getSearchObject(int upper, int lower) {
            std::stringstream download;
            bool first = true;
            if (!(upper == 0 && lower == 0)) {
                if (upper > lower) {
                    for (unsigned int i = upper; i > lower; i--) {
                        if (!first) {
                            download << ",";
                        }
                        if (i < BrType::LevelID.size()) { 
                            download << std::to_string(BrType::LevelID.at(i));
                            first = false;
                        } 
                    }
                }
            } else {
                if (!BrType::LevelID.empty()) {
                    download << std::to_string(BrType::LevelID.at(0));
                }
            }
            log::debug("Dowload: {}", download.str() );
            
            download << "&gameVersion=22";
            GJSearchObject* searchObj = GJSearchObject::create(SearchType::Type19, download.str());
            return searchObj;
        }   

        
};

#endif