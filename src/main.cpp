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
		CCNode* Filter;
		Filter = this->getChildByID("other-filter-menu");
		if (!Filter) {
			 if (!Loader::get()->isModLoaded("geode.node-ids")) {
				 Filter = this->getChildByType<CCMenu>(0);
				if (!Filter) {
					return true;
				}
				// bug fix
				auto winSize = CCDirector::get()->getWinSize();
				Filter->setLayout(
					ColumnLayout::create()
						->setAxisReverse(true)
						->setGap(10)
						->setAxisAlignment(AxisAlignment::End)
				);
				Filter->setAnchorPoint({1, 0.5f});
				Filter->setPosition({winSize.width - 5, Filter->getPositionY()});
				Filter->setContentSize({Filter->getContentSize().width, winSize.height-10});
			 } else {return true;}
		}
		auto BRL_Button = CCMenuItemSpriteExtra::create(
			 CircleButtonSprite::createWithSprite(
                "list_icon.png"_spr,
                1.1,
                CircleBaseColor::DarkPurple,
                CircleBaseSize::Small
            ),
			this,
			menu_selector(HookSearch::pushbtncustom)
		);
		BRL_Button->setID("BRL/Button"_spr);
		Filter->addChild(BRL_Button);
		Filter->updateLayout();
		return true;
	}
	void pushbtncustom(CCObject*) {
		change_scene();
    }
};

class $modify(MenuLayer) {
    bool init() {
		if (level_map.empty()) {
				getlistjson([=](matjson::Value response) {
					int order = 0;
					for (const auto& item : response.asArray().unwrap()) {
						order+=1;
						int curord = order;
						getleveljson(item.asString().unwrap(), [=](matjson::Value response) {
							level_map[curord] = response;
						});
					}
			},nullptr);
		}
		if (BRPacks::PacksIDs.empty()) {
				getpackjson([=](matjson::Value response) {
					for (auto item : response.asArray().unwrap()) {
						log::debug("{}", item.dump());
						for (const auto& lels : item["levels"].asArray().unwrap()) {
							getleveljson(lels.asString().unwrap(), [=](matjson::Value response) {
								BRPacks::level_map[lels.asString().unwrap()] = response;
								BRPacks::levelid_map[response["id"].asInt().unwrap()] = response;
							});
						}
						BRPacks::PacksIDs.emplace_back(item["name"].asString().unwrap(),item["levels"]);
					}
			},nullptr);
		}
        return MenuLayer::init();
    }
};
