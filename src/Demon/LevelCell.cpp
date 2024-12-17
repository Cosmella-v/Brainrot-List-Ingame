#include <Geode/Geode.hpp>
#include <Geode/modify/LevelCell.hpp>
#include "../CustomSearch/BrType.hpp"
#include "../url.hpp"
using namespace geode::prelude;
/*
#include <Geode/modify/SetIDPopup.hpp>
class $modify(SetIDPopup) {
	bool init(int current, int begin, int end, gd::string title, gd::string button, bool p5, int p6, float y, bool p8, bool p9) {
		log::debug("{} {} {} {} {} {} {} {} {} {}",current,begin,end,title,button,p5,p6,y,p8,p9);
		return SetIDPopup::init(current,begin,end,title,button,p5,p6,y,p8,p9);
	};

};
*/

class $modify(LevelCell) {
	static void onModify(auto& self) {
        (void)self.setHookPriority("LevelCell::loadFromLevel", INT_MIN/2-1); 
    }
	void loadFromLevel(GJGameLevel* p0) {
		 if (!BrType::LoadedAllLevels) {
            BrType::parseRequestString(level_map);
        }
		LevelCell::loadFromLevel(p0);
		if (CCNode* Boo = this->getChildByIDRecursive("uproxide.more_difficulties/more-difficulties-spr")) {
			Boo->setScale(0);
			Boo->setVisible(false);
			typeinfo_cast<CCSprite*>(Boo)->setOpacity(0);
		}

		auto difficultyNode = m_mainLayer->getChildByID("difficulty-container");
		if (!difficultyNode) {
			return;
		};
		CCNode* difficultySpr = difficultyNode->getChildByID("difficulty-sprite");
		if (!difficultySpr) {
			return;
		}
		cocos2d::CCPoint difficultyPos = difficultySpr->getPosition() + CCPoint { .0f, .0f };
		if (difficultyNode) {
			int demonpos = BrType::find(p0->m_levelID);
			if (demonpos != -1) {
                CCSprite* mdSpr = CCSprite::create("Difficulty_Brl.png"_spr);
				mdSpr->setPosition(difficultyPos);
				if (mdSpr->getParent() != difficultyNode) difficultyNode->addChild(mdSpr);
				mdSpr->setZOrder(difficultySpr->getZOrder());
				typeinfo_cast<CCSprite*>(difficultySpr)->setOpacity(0);
				mdSpr->setID("BRL"_spr);
			}

		} 
	}
}; 