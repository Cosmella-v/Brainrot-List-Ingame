#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include "../CustomSearch/BrType.hpp"
#include "../url.hpp"
#include "../CustomSearch/Packlist.hpp"
using namespace geode::prelude;

class $modify(moddedLevelInfoLayer,LevelInfoLayer) {
	static void onModify(auto& self) {
        (void)self.setHookPriority("LevelInfoLayer::init", INT_MIN/2-1); 
    }
	void YoutubeLink(CCObject*) {
		std::string VideoLink = WebviewUrl::ConvertToEmbed(this->m_fields->jsonSavedBRL["verification"].asString().unwrapOr("f"));
		if (VideoLink != "f") {
			WebviewUrl::Open(VideoLink,this->m_level->m_levelName);
		};
	}
	struct Fields {
		matjson::Value jsonSavedBRL;
    };
	bool init(GJGameLevel* p0, bool p1) {
		if (!LevelInfoLayer::init(p0, p1)) {
			return false;
		}
		if (!m_difficultySprite) {
			return true;
		}
		cocos2d::CCPoint difficultyPos = m_difficultySprite->getPosition() + CCPoint { .0f, .0f };
		int zOrder = m_difficultySprite->getZOrder();
		auto demonpos = BrType::find(p0->m_levelID);
		if (!demonpos) {
			demonpos = BRPacks::find(p0->m_levelID);
		}
		if (demonpos) {
			if (CCNode* Boo = this->getChildByIDRecursive("uproxide.more_difficulties/more-difficulties-spr")) {
				Boo->setScale(0);
				Boo->setVisible(false);
				typeinfo_cast<CCSprite*>(Boo)->setOpacity(0);
			}
			this->m_fields->jsonSavedBRL = std::get<2>(*demonpos);
			CCNode* rightside = this->getChildByIDRecursive("favorite-button");
			if (!rightside) {
				if (!Loader::get()->isModLoaded("geode.node-ids")) {
					 if (auto menu = this->getChildByType<CCMenu>(3)) {
						if (auto node = menu->getChildByType<CCNode>(1)) {
							rightside = node;
						} else {
							goto after;
						}
					 } else {
						goto after;
					 }
				} else {goto after;};
			}
			if (this->m_fields->jsonSavedBRL.contains("verification")) {
				auto btn = CCSprite::createWithSpriteFrameName("gj_ytIcon_001.png");
				btn->setScale(0.75);
				auto btnee = CCMenuItemSpriteExtra::create(btn, this, menu_selector(moddedLevelInfoLayer::YoutubeLink));
				btnee->setPosition(rightside->getPosition() + ccp(0,rightside->getContentHeight()+2));
				rightside->getParent()->addChild(btnee);
			}
			after:
				CCSprite* mdSpr = (!Mod::get()->getSettingValue<bool>("better-face")) ? CCSprite::create("normal_face_with_demon_text.png"_spr) : CCSprite::create("Betterface.png"_spr);
				mdSpr->setPosition(difficultyPos);
				if (mdSpr->getParent() != this) this->addChild(mdSpr);
				mdSpr->setZOrder(zOrder);
				mdSpr->setScale((!Mod::get()->getSettingValue<bool>("better-face")) ? 0.2 : 0.4);
				m_difficultySprite->setOpacity(0);
				mdSpr->setID("BRL"_spr);
		}
		return true;
	}
};