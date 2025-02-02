#include <Geode/Geode.hpp>
#include <Geode/modify/LevelCell.hpp>
#include "../CustomSearch/BrType.hpp"
#include "../url.hpp"
#include "../CustomSearch/Packlist.hpp"
using namespace geode::prelude;
/*
#include <Geode/modify/SetIDPopup.hpp>
class $modify(SetIDPopup) {
	bool init(int current, int begin, int end, gd::string title, gd::string button, bool p5, int p6, float y, bool p8, bool p9) {
		log::debug("{} {} {} {} {} {} {} {} {} {}",current,begin,end,title,button,p5,p6,y,p8,p9);
		return SetIDPopup::init(current,begin,end,title,button,p5,p6,y,p8,p9);
	}
};
*/

class $modify(LevelCell) {
	static void onModify(auto& self) {
        (void)self.setHookPriority("LevelCell::loadFromLevel", -3998);
    }
	void loadFromLevel(GJGameLevel* p0) {
		if (!BrType::LoadedAllLevels) {
			BrType::parseRequestString(level_map);
		}
		LevelCell::loadFromLevel(p0);
		if (Mod::get()->getSettingValue<bool>("brl-layer-only")) {
			auto levelBrowserLayer = CCScene::get()->getChildByType<LevelBrowserLayer>(0);
			if (levelBrowserLayer) {
				if (!levelBrowserLayer->getUserObject("modified-by-brl"_spr)) return; // for full BRL list layer
				else if (levelBrowserLayer->m_list) {
					auto label = levelBrowserLayer->m_list->getChildByType<CCLabelBMFont>(0);
					if (label) {
						std::string mapPackName = label->getString();
						for (auto pack : BRPacks::PacksIDs) {
							if (std::get<0>(pack) == mapPackName) {
								log::info("THIS IS A BRL MAP PACK");
								break;
							}
						}
					}
				}
			}
		}
		CCNode* difficultyNode = m_mainLayer->getChildByID("difficulty-container");
		if (!difficultyNode) return;
		CCNode* difficultySpr = difficultyNode->getChildByID("difficulty-sprite");
		if (!difficultySpr) return;

		cocos2d::CCPoint difficultyPos = difficultySpr->getPosition() + CCPoint { .0f, .0f };
		auto demonpos = BrType::find(p0->m_levelID);
		if (!demonpos) demonpos = BRPacks::find(p0->m_levelID);
		if (!demonpos) return; // if it's STILL nullptr, abort
		if (CCNode* moreDifficultiesSprite = this->getChildByIDRecursive("uproxide.more_difficulties/more-difficulties-spr")) {
			moreDifficultiesSprite->setScale(0);
			moreDifficultiesSprite->setVisible(false);
			static_cast<CCSprite*>(moreDifficultiesSprite)->setOpacity(0);
		}
		CCSprite* mdSpr = (!Mod::get()->getSettingValue<bool>("better-face")) ? CCSprite::create("normal_face_with_demon_text.png"_spr) : CCSprite::create("Betterface.png"_spr);
		mdSpr->setPosition(difficultyPos);
		if (mdSpr->getParent() != difficultyNode) difficultyNode->addChild(mdSpr);
		mdSpr->setZOrder(difficultySpr->getZOrder());
		mdSpr->setScale((!Mod::get()->getSettingValue<bool>("better-face")) ? 0.2 : 0.4);
		mdSpr->setAnchorPoint({0.5,0.350});
		typeinfo_cast<CCSprite*>(difficultySpr)->setOpacity(0);
		mdSpr->setID("brl-difficulty-sprite"_spr); // more descriptive and kebab case node ID
	}
}; 