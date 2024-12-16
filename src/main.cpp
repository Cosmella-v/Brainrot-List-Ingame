#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "url.hpp" 
#include "CustomSearch/CustomSearch.hpp" 

using namespace geode::prelude;

class $modify(MyMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) {
            return false;
        }
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

		auto myButton = CCMenuItemSpriteExtra::create(
			CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
			this,
			menu_selector(MyMenuLayer::onMyButton)
		);

		auto menu = this->getChildByID("bottom-menu");
		menu->addChild(myButton);

		myButton->setID("my-button"_spr);

		menu->updateLayout();



        return true;
    }

    void onMyButton(CCObject*) {
		change_scene();
    }
};
