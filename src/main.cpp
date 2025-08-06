#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>
#include "GetData.hpp"
#include "CustomSearch/CustomSearch.hpp" 

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
$on_mod(Loaded)
{
Loader::get()->queueInMainThread([=]{
		brlist::MainList();
		brlist::Packs();
	});
}

