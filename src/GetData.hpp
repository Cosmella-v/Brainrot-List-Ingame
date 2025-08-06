#pragma once
#include "url.hpp" 
#include "CustomSearch/Packlist.hpp" 
#include "CustomSearch/CustomSearch.hpp" 

class brlist {
static void MainList(std::function<void()> failed = [](){}) {
if (level_map.empty()) {
				getBRListJSON([=](matjson::Value response) {
					if (!response.isArray()) {
						return log::error("reading json is not expected (array expected)");
					};
					int order = 0;
					for (const auto& item : response.asArray().unwrap()) {
						if (!item.isString()) { 
							return log::error("reading json level map for levels (main list) is not expected (string expected)");
						}
						order += 1;
						int curord = order;
						getBRLevelJSON(item.asString().unwrap(), [=](matjson::Value response) {
							level_map[curord] = response;
						});
					}
				}, failed);
		}
}
static void Packs(std::function<void()> failed = [](){}) {
    if (BRPacks::PacksIDs.empty()) {
				getBRPackJSON([=](matjson::Value response) {
					if (!response.isArray()) {
						return log::error("reading json packs is not expected (array expected)");
					}
					for (auto item : response.asArray().unwrap()) {
						//log::debug("{}", item.dump());
						if (!item.contains("name") || !item["name"].isString()) { 
							return log::error("reading json pack name for levels is not expected (string expected)");
						}

                        if (!item.contains("levels") || !item["levels"].isArray()) {
							return log::error("reading json levels (pack) is not expected (array expected)");
                        }

						for (const auto& lels : item["levels"].asArray().unwrap()) {
                            if (lels.isString()) {
                                getBRLevelJSON(lels.asString().unwrap(), [=](matjson::Value response) {
                                    if (!response.contains("id") || !response["id"].isNumber()) { return log::error("reading json id for levels is not expected (number expected)"); }
                                    BRPacks::level_map[lels.asString().unwrap()] = response;
                                    BRPacks::levelid_map[response["id"].asInt().unwrap()] = response;
                                });
                            }
                            BRPacks::PacksIDs.emplace_back(item["name"].asString().unwrap(),item["levels"]);
                        }
					}
				}, failed);
    	}
}
}