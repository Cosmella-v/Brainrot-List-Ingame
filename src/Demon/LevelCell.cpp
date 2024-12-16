#include <Geode/Geode.hpp>
#include <Geode/modify/LevelCell.hpp>
#include "../CustomSearch/BrType.hpp"
#include "../url.hpp"
using namespace geode::prelude;

class $modify(LevelCell) {
	void loadFromLevel(GJGameLevel* p0) {
		LevelCell::loadFromLevel(p0);
		 if (BrType::firstTimeOpen) {
                BrType::parseRequestString(level_map);
                BrType::firstTimeOpen = false;
                BrType::filterType = -1;
        }

		auto difficultyNode = m_mainLayer->getChildByID("difficulty-container");
        GJDifficultySprite* difficultySpr = static_cast<GJDifficultySprite*>(difficultyNode->getChildByID("difficulty-sprite"));

		cocos2d::CCPoint difficultyPos = difficultySpr->getPosition() + CCPoint { .0f, .0f };
		if (difficultyNode) {
			if (BrType::find(p0->m_levelID) ) {
                CCSprite* mdSpr = CCSprite::create("Difficulty_Brl.png"_spr);
				mdSpr->setPosition(difficultyPos);
				if (mdSpr->getParent() != difficultyNode) difficultyNode->addChild(mdSpr);
				mdSpr->setZOrder(difficultySpr->getZOrder());
				difficultySpr->setOpacity(0);
			}

		} 
	}
}; 