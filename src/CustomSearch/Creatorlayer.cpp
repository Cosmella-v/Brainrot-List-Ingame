#include <Geode/Geode.hpp>
#include <Geode/modify/CreatorLayer.hpp>

using namespace geode::prelude;
class $modify(HookCreator, CreatorLayer) {
	static void onModify(auto& self) {
        (void)self.setHookPriority("CreatorLayer::init", INT_MIN/2-1); 
    }
    void prompt(CCObject* x) {
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
			"Well scram it's not finished!",
			18, 1.0f, false, ccWHITE
		));
        DialogLayer* dialog = DialogLayer::createWithObjects(dialogLines, 2);
		dialog->updateChatPlacement(DialogChatPlacement::Center);
		dialog->animateInRandomSide();
		dialog->addToMainScene();
    }
	bool init() {
		if (!CreatorLayer::init()) return false;

		CCNode* Filter = this->getChildByID("bottom-left-menu");
		if (!Filter) {
			 if (!Loader::get()->isModLoaded("geode.node-ids")) {
				auto winSize = CCDirector::get()->getWinSize();
				Filter = CCMenu::create();
				Filter->setPosition(3, 3);
				Filter->setID("bottom-left-menu");
				Filter->setAnchorPoint({0, 0});
				Filter->setZOrder(1);
				Filter->setContentSize({ 40.f, winSize.height/2 });
				Filter->setLayout(
					ColumnLayout::create()
						->setAxisAlignment(AxisAlignment::Start)
				);
				this->addChild(Filter);
			 } else {return true;}
		}

        auto BRL_Button = CCMenuItemSpriteExtra::create(
			 CircleButtonSprite::createWithSprite(
                "list_icon.png"_spr,
                1.1,
               (Mod::get()->getSettingValue<bool>("dark-mode")) ? CircleBaseColor::DarkPurple : CircleBaseColor::Green,
                CircleBaseSize::Small
            ),
			this,
			menu_selector(HookCreator::prompt)
		);
		BRL_Button->setID("BRL/roulette"_spr);
		Filter->addChild(BRL_Button);
		Filter->updateLayout();
        return true;
    }
};
