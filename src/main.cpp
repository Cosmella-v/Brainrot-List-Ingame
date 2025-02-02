#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "url.hpp" 
#include "CustomSearch/Packlist.hpp" 
#include "CustomSearch/CustomSearch.hpp" 
#include <Geode/modify/LevelSearchLayer.hpp>

using namespace geode::prelude;

class $modify(HookSearch, LevelSearchLayer) {
	bool init(int in) {
		if (!LevelSearchLayer::init(in)) {
			return false;
		}
		CCNode* otherFilterMenu;
		otherFilterMenu = this->getChildByID("other-filter-menu");
		if (!otherFilterMenu) return true;
		auto BRL_Button = CCMenuItemSpriteExtra::create(
			 CircleButtonSprite::createWithSprite(
                "list_icon.png"_spr, 1.1,
                (Mod::get()->getSettingValue<bool>("dark-mode")) ? CircleBaseColor::DarkPurple : CircleBaseColor::Green,
                CircleBaseSize::Small
            ),
			this,
			menu_selector(HookSearch::onBRLButton)
		);
		BRL_Button->setID("brl-button"_spr); // use kebab-case for node IDs
		otherFilterMenu->addChild(BRL_Button);
		otherFilterMenu->updateLayout();
		return true;
	}
	void onBRLButton(CCObject*) {
		switchToBRLScene();
    }
};

class $modify(MenuLayer) {
    bool init() {
		if (!MenuLayer::init()) return false;
		if (level_map.empty()) {
			std::thread([=] {  
				getlistjson([=](matjson::Value response) {
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
						getleveljson(item.asString().unwrap(), [=](matjson::Value response) {
							level_map[curord] = response;
						});
					}
				}, nullptr);
			}).detach();
		}
		if (BRPacks::PacksIDs.empty()) {
			std::thread([=] {  
				getpackjson([=](matjson::Value response) {
					if (!response.isArray()) {
						return log::error("reading json packs is not expected (array expected)");
					}
					for (auto item : response.asArray().unwrap()) {
						//log::debug("{}", item.dump());
						if (!item.contains("levels") || !item["levels"].isArray()) {
							return log::error("reading json levels (pack) is not expected (array expected)");
						}

						if (!item.contains("name") || !item["name"].isString()) { 
							return log::error("reading json pack name for levels is not expected (string expected)");
						}

						for (const auto& lels : item["levels"].asArray().unwrap()) {
							getleveljson(lels.asString().unwrap(), [=](matjson::Value response) {
								if (!response.contains("id") || !response["id"].isNumber()) { return log::error("reading json id for levels is not expected (number expected)"); }
								BRPacks::level_map[lels.asString().unwrap()] = response;
								BRPacks::levelid_map[response["id"].asInt().unwrap()] = response;
							});
						}
						BRPacks::PacksIDs.emplace_back(item["name"].asString().unwrap(),item["levels"]);
					}
				}, nullptr);
			}).detach();
    	}
		return true;
	}
};
