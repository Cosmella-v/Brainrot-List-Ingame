#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include "../CustomSearch/BrType.hpp"
#include "../url.hpp"
using namespace geode::prelude;


class $modify(LevelInfoLayer) {
	static void onModify(auto& self) {
        (void)self.setHookPriority("LevelInfoLayer::init", INT_MIN/2-1); 
    }
	bool init(GJGameLevel* p0, bool p1) {
		if (!LevelInfoLayer::init(p0, p1)) {
			return false;
		}
		if (!m_difficultySprite) {
			return true;
		}
		if (CCNode* Boo = this->getChildByIDRecursive("uproxide.more_difficulties/more-difficulties-spr")) {
			Boo->setScale(0);
			Boo->setVisible(false);
			typeinfo_cast<CCSprite*>(Boo)->setOpacity(0);
		}
		cocos2d::CCPoint difficultyPos = m_difficultySprite->getPosition() + CCPoint { .0f, .0f };
		int zOrder = m_difficultySprite->getZOrder();
		auto demonpos = BrType::find(p0->m_levelID);
		if (demonpos) {
			log::debug("{}",std::get<2>(*demonpos).dump());

            CCSprite* mdSpr = CCSprite::create("normal_face_with_demon_text.png"_spr);
			mdSpr->setPosition(difficultyPos);
			if (mdSpr->getParent() != this) this->addChild(mdSpr);
			mdSpr->setZOrder(zOrder);
			mdSpr->setScale(0.2);
			m_difficultySprite->setOpacity(0);
			mdSpr->setID("BRL"_spr);
		}
		return true;
	}
};