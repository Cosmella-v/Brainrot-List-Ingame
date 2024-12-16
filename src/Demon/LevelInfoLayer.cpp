#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include "../CustomSearch/BrType.hpp"
#include "../url.hpp"
using namespace geode::prelude;


class $modify(LevelInfoLayer) {
	bool init(GJGameLevel* p0, bool p1) {
		if (!LevelInfoLayer::init(p0, p1)) {
			return false;
		}
		cocos2d::CCPoint difficultyPos = m_difficultySprite->getPosition() + CCPoint { .0f, .0f };
		int zOrder = m_difficultySprite->getZOrder();
		if (BrType::find(p0->m_levelID) ) {
            CCSprite* mdSpr = CCSprite::create("list_icon.png"_spr);
			mdSpr->setPosition(difficultyPos);
            mdSpr->setScale(0.5);
            mdSpr->setAnchorPoint({0.5,0.8});
			if (mdSpr->getParent() != this) this->addChild(mdSpr);
			mdSpr->setZOrder(zOrder);
			m_difficultySprite->setOpacity(0);
		}
		return true;
	}
};