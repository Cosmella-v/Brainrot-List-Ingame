#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include "../CustomSearch/BrType.hpp"
#include "../url.hpp"
#include "../CustomSearch/Packlist.hpp"
using namespace geode::prelude;

class $modify(moddedLevelInfoLayer,LevelInfoLayer) {
	static void onModify(auto& self) {
        (void)self.setHookPriority("LevelInfoLayer::init", -3998); 
    }
	void onYTVerification(CCObject*) {
		if (!this->m_fields->jsonSavedBRL.contains("verification") || !this->m_fields->jsonSavedBRL["verification"].isString()) {
			return; // the field is wrong smh
		};
		std::string VideoLink = WebviewUrl::ConvertToEmbed(this->m_fields->jsonSavedBRL["verification"].asString().unwrapOr("failure"));
		if (VideoLink == "failure") return;
		WebviewUrl::Open(VideoLink,this->m_level->m_levelName);
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
			if (CCNode* moreDifficultiesSprite = this->getChildByIDRecursive("uproxide.more_difficulties/more-difficulties-spr")) {
				moreDifficultiesSprite->setScale(0);
				moreDifficultiesSprite->setVisible(false);
				typeinfo_cast<CCSprite*>(moreDifficultiesSprite)->setOpacity(0);
			}
			this->m_fields->jsonSavedBRL = std::get<2>(*demonpos);
			CCNode* menuForYTButton = nullptr;

			// fuck death tracker, Why must it be like this!
			auto deathTracker = Loader::get()->getLoadedMod("elohmrow.death_tracker");
			if (deathTracker) {
				if (!deathTracker->getSettingValue<bool>("left-menu")) {
					menuForYTButton = this->getChildByIDRecursive("dt-skull-button"); 
				}
			};

			if (!menuForYTButton) menuForYTButton = this->getChildByIDRecursive("favorite-button");
			if (this->m_fields->jsonSavedBRL.contains("verification")) {
				auto buttonSprite = CCSprite::createWithSpriteFrameName("gj_ytIcon_001.png");
				buttonSprite->setScale(0.75);
				auto button = CCMenuItemSpriteExtra::create(buttonSprite, this, menu_selector(moddedLevelInfoLayer::onYTVerification));
				button->setPosition(menuForYTButton->getPosition() + ccp(0,menuForYTButton->getContentHeight()+2));
				button->setID("brl-youtube-verification"_spr);
				menuForYTButton->getParent()->addChild(button);
				menuForYTButton->updateLayout();
			}
			CCSprite* mdSpr = (!Mod::get()->getSettingValue<bool>("better-face")) ? CCSprite::create("normal_face_with_demon_text.png"_spr) : CCSprite::create("Betterface.png"_spr);
			mdSpr->setPosition(difficultyPos);
			if (mdSpr->getParent() != this) this->addChild(mdSpr);
			mdSpr->setZOrder(zOrder);
			mdSpr->setScale((!Mod::get()->getSettingValue<bool>("better-face")) ? 0.2 : 0.4);
			m_difficultySprite->setOpacity(0);
			mdSpr->setID("brl-difficulty-sprite"_spr);
		}
		return true;
	}
};