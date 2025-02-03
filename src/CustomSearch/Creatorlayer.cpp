#include <Geode/Geode.hpp>
#include <Geode/modify/CreatorLayer.hpp>

using namespace geode::prelude;
class $modify(HookCreator, CreatorLayer) {
	static void onModify(auto& self) {
        (void)self.setHookPriority("CreatorLayer::init", -3998); 
    }
    void prompt(CCObject*) {
        auto dialogLines = CCArray::create();
        dialogLines->addObject(DialogObject::create(
			"THE KEYMASTER",
			"<d010>.<d010>.<d010>.",
			2, 1.0f, false, ccWHITE
		));
        dialogLines->addObject(DialogObject::create(
			"THE KEYMASTER",
			"Did you really think you could play roulette?<d010><d010><d010>",
			17, 1.0f, false, ccWHITE
		));
        dialogLines->addObject(DialogObject::create(
			"THE KEYMASTER",
			"Well, scram! It's not finished!",
			18, 1.0f, false, ccWHITE
		));
        DialogLayer* dialog = DialogLayer::createWithObjects(dialogLines, 2);
		dialog->updateChatPlacement(DialogChatPlacement::Center);
		dialog->animateInRandomSide();
		dialog->addToMainScene();
    }
	bool init() {
		if (!CreatorLayer::init()) return false;

		CCNode* bottomLeftMenu = this->getChildByID("bottom-left-menu");
		if (!bottomLeftMenu) return true;

        auto brlRouletteButton = CCMenuItemSpriteExtra::create(
			 CircleButtonSprite::createWithSprite(
                "list_icon.png"_spr, 1.1,
                (Mod::get()->getSettingValue<bool>("dark-mode")) ? CircleBaseColor::DarkPurple : CircleBaseColor::Green,
                CircleBaseSize::Small
            ),
			this,
			menu_selector(HookCreator::prompt)
		);

		brlRouletteButton->setID("brl-roulette"_spr);
		bottomLeftMenu->addChild(brlRouletteButton);
		bottomLeftMenu->updateLayout();
        return true;
    }
};