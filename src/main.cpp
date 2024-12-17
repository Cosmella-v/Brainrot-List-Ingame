#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "url.hpp" 
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
		  Filter = this->getChildByType<CCMenu>(0);
		  if (!Filter) {
			return true;
		  }
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
		BRL_Button->setID("BLR/Button"_spr);
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
			});
		}
        return MenuLayer::init();
    }
};
