#pragma once
#include <Geode/Bindings.hpp>
#include <Geode/modify/DemonFilterSelectLayer.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/Geode.hpp>
#include "BrType.hpp"
#include <fmt/core.h>
#include <fmt/format.h>
#include <Geode/modify/GJSearchObject.hpp>
#include <fmt/chrono.h>
#include <sstream>
#include <time.h>
#include <chrono>
#include "Packlist.hpp"
#include <Geode/modify/MapPackCell.hpp>
#ifndef GEODE_IS_ANDROID
#include <sys/timeb.h>
#endif
double timeBD = 0;
double m_stopratelimit = 0;

using namespace std::chrono;
class $modify(FixMapPackCell, MapPackCell) {
    struct Fields {
        bool m_moddedcellforBrl = false;
    };
    void loadFromMapPack(GJMapPack* cell) {
        MapPackCell::loadFromMapPack(cell);
        if (cell->getUserObject("brl_modified")) {
            this->m_fields->m_moddedcellforBrl = true;
            this->m_viewButton->m_pfnSelector = (cocos2d::SEL_MenuHandler)(&FixMapPackCell::Better_onClick);
        }
        return;
    };

    void Better_onClick(CCObject*) {
        log::debug("Should load new scene");
        BrType::ShouldChangeText = this->m_mapPack->m_packName;
        auto browserLayer = LevelBrowserLayer::scene(GJSearchObject::create(SearchType::Type19, this->m_mapPack->m_levelStrings));
        CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, browserLayer));
    }

};

using namespace geode::prelude;
static void change_scene() {
        if (!BrType::LoadedAllLevels) {
            BrType::parseRequestString(level_map);
            BrType::filterType = -1;
        }
        BrType::isSearchingBR = true;
        //BrType::MapPack_Br = true;
        auto browserLayer = LevelBrowserLayer::scene(BrType::getSearchObject(10, 0,false));
        CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, browserLayer));
}

double getFullDoubleTime() {
    #ifdef GEODE_IS_ANDROID
        struct timespec ts;
        clock_gettime(0, &ts);
        return ts.tv_sec + ts.tv_nsec / 1000000000.0;
    #else
        struct timeb timebuffer;
        ftime(&timebuffer);
        return timebuffer.time + timebuffer.millitm / 1000.0;
    #endif
}


class $modify(BRlist, LevelBrowserLayer) {
    static void onModify(auto& self) {
        (void)self.setHookPriority("LevelBrowserLayer::init", INT_MIN/2-1); 
    }
    struct Fields {
        int m_currentPage = 0;
        int m_furthestLoadedPage = 0;
        int m_lowIdx = 0;
        bool isBrainrot = false;
        bool MapPack_Br = false;
        gd::string mapPackText;
    };
    bool init(GJSearchObject* p0) {
        this->m_fields->isBrainrot = BrType::isSearchingBR;
        this->m_fields->mapPackText = BrType::ShouldChangeText;
        this->m_fields->MapPack_Br = BrType::MapPack_Br;
        BrType::MapPack_Br = false;
        BrType::ShouldChangeText = "";
        if (this->m_fields->MapPack_Br) {
            bool inits = LevelBrowserLayer::init(BrType::getSearchObject(10, 0, this->m_fields->MapPack_Br));
            updText();
            return inits;
        }
        if (!this->m_fields->isBrainrot) {
            bool inits = LevelBrowserLayer::init(p0);
            updText();
            return inits;
        }

        if (p0->m_searchType != SearchType::Type19 && p0->m_searchType != SearchType::MapPack) {
            return LevelBrowserLayer::init(p0);
        }
        BrType::isSearchingBR = false;

        this->m_fields->m_currentPage = 0;
        int page = this->m_fields->m_currentPage;
        this->m_fields->m_lowIdx = page * 10;
        this->setUserObject("brl_modified", CCBool::create(true));
        LevelBrowserLayer::init(BrType::getSearchObject(10, 0, this->m_fields->MapPack_Br));
        hideStuff();
        return true;
    }
    void updText() {
        if (!this->m_fields->mapPackText.empty()) {
                if (CCNode* CFix = this->m_list) {
                    if (!Loader::get()->isModLoaded("geode.node-ids")) {
                        if (CCLabelBMFont* TextLab = CFix->getChildByType<CCLabelBMFont>(0)) {
                            TextLab->setString(this->m_fields->mapPackText.c_str());
                        };
                    } else {
                        if (CCNode* TextLab = CFix->getChildByID("title")) {
                            typeinfo_cast<CCLabelBMFont*>(TextLab)->setString(this->m_fields->mapPackText.c_str());
                        };
                    }
                }
        }
    }
    void loadLevelsFinished(cocos2d::CCArray* p0, char const* p1, int p2) {
        if (this->m_fields->MapPack_Br && this->m_searchObject->m_searchType == SearchType::MapPack) {
            p0 = BRPacks::GetPacks();
        } 
        LevelBrowserLayer::loadLevelsFinished(p0, p1, p2);
        updText();
        if (!this->m_fields->isBrainrot) {
            return;
        }
        if (this->m_searchObject->m_searchType != SearchType::Type19 && this->m_searchObject->m_searchType  != SearchType::MapPack) {
            return;
        }
        auto prevBtn = this->m_leftArrow;
        auto nextBtn = this->m_rightArrow;

        hideStuff();

        prevBtn->setVisible(true);
        nextBtn->setVisible(true);
        this->m_pageBtn->setVisible(true);

        if (this->m_fields->m_currentPage <= 0) {
            prevBtn->setVisible(false);
        } else if (this->m_fields->m_currentPage >= (BrType::LevelID.size() / 10)) {
            nextBtn->setVisible(false);
        }
    }
    void lastpage(auto h) {
        if (this->m_fields->m_currentPage == (BrType::LevelID.size() / 10)) {
            return;
        };
        this->m_fields->m_currentPage = (BrType::LevelID.size() / 10);
        nextBtnActions();
    }

    void firstpage(auto h) {
         if (this->m_fields->m_currentPage == 0) {
            return;
        };
        this->m_fields->m_currentPage = 0;
        nextBtnActions();
    }

    void onNextPage(CCObject* sender) {
        updText();
        if (!this->m_fields->isBrainrot) {
            return LevelBrowserLayer::onNextPage(sender);
        }
        if (this->m_searchObject->m_searchType != SearchType::Type19) {
            return LevelBrowserLayer::onNextPage(sender);
        }

        if (this->m_fields->m_currentPage < (BrType::LevelID.size() / 10)) {
            this->m_fields->m_currentPage += 1;
        }
        nextBtnActions();
        
    }
    void setIDPopupClosed(SetIDPopup* popup, int p1) {
        if (!this->m_fields->isBrainrot) {
            return LevelBrowserLayer::setIDPopupClosed(popup,p1);
        }
        if (this->m_searchObject->m_searchType != SearchType::Type19 && this->m_searchObject->m_searchType != SearchType::MapPack) {
            return LevelBrowserLayer::setIDPopupClosed(popup,p1);
        }
        p1-=1;
        if (this->m_fields->m_currentPage == p1) {
            return;
        }
        if (p1 >= (BrType::LevelID.size() / 10)) {
           return lastpage(nullptr);
        };
        this->m_fields->m_currentPage = p1;

        nextBtnActions();
    }
    void onGoToPage(CCObject* sender) {
        if (!this->m_fields->isBrainrot) {
            return LevelBrowserLayer::onGoToPage(sender);
        }
        if (this->m_searchObject->m_searchType != SearchType::Type19 && this->m_searchObject->m_searchType  != SearchType::MapPack) {
            return LevelBrowserLayer::onGoToPage(sender);
        }
        SetIDPopup* popup = SetIDPopup::create(this->m_fields->m_currentPage+1,1,(BrType::LevelID.size() / 10)+1,"Go to Page","OK",true,1,60,false,false);
        popup->m_delegate = this;
        popup->show();
    }

    void onPrevPage(CCObject* sender) {
        updText();
        if (!this->m_fields->isBrainrot) {
            return LevelBrowserLayer::onPrevPage(sender);
        }
        if (this->m_searchObject->m_searchType != SearchType::Type19 && this->m_searchObject->m_searchType  != SearchType::MapPack) {
            return LevelBrowserLayer::onPrevPage(sender);
        }
        if (this->m_fields->m_currentPage > 0) {
            this->m_fields->m_currentPage -= 1;
        }
        nextBtnActions();
        
    }
    void loadPage(GJSearchObject* type) {
        LevelBrowserLayer::loadPage(type);
        updText();
        return;
    }
    void fixtimeout(auto h) {
        m_stopratelimit = getFullDoubleTime() + timeBD;
        BRlist::loadPage(BrType::getSearchObject( ((BrType::LevelID.size() - 10 - this->m_fields->m_currentPage * 10) - BrType::LevelID.size()) *-1, (((BrType::LevelID.size()) - this->m_fields->m_currentPage * 10) - BrType::LevelID.size()) *-1,this->m_fields->MapPack_Br ));
        this->m_pageBtn->setVisible(true);
        if (CCNode* first_Betterinfo = this->getChildByIDRecursive("cvolton.betterinfo/first-button")) {
            first_Betterinfo->setVisible(this->m_fields->m_currentPage != 0);
        }
        hideStuff();
    }
    void nextBtnActions() {
        hideStuff();
        timeBD = 0.85 - (getFullDoubleTime() - m_stopratelimit);
        if(timeBD < 0) {
            timeBD = 0;
            fixtimeout(nullptr);
        } else {
            if (CCNode* CFix = this->m_list->m_listView) {
                CFix->setVisible(false);
            }
            this->m_circle->setVisible(true);
            this->getScheduler()->scheduleSelector((cocos2d::SEL_SCHEDULE)(&BRlist::fixtimeout), this, 1, 0, timeBD, false);
        }
    }
    void hideStuff() {
        if (CCNode* last_Betterinfo = this->getChildByIDRecursive("cvolton.betterinfo/last-button")) {
            typeinfo_cast<CCMenuItemSpriteExtra*>(last_Betterinfo)->m_pfnSelector = (cocos2d::SEL_MenuHandler)(&BRlist::lastpage);
             last_Betterinfo->setVisible(this->m_fields->m_currentPage < (BrType::LevelID.size() / 10));
        }
        if (CCNode* filter_Betterinfo = this->getChildByIDRecursive("cvolton.betterinfo/filter-button")) {
            filter_Betterinfo->setVisible(false);
        }
        if (CCNode* first_Betterinfo = this->getChildByIDRecursive("cvolton.betterinfo/first-button")) {
            typeinfo_cast<CCMenuItemSpriteExtra*>(first_Betterinfo)->m_pfnSelector = (cocos2d::SEL_MenuHandler)(&BRlist::firstpage);
            first_Betterinfo->setVisible(this->m_fields->m_currentPage != 0);
        }
        auto prevBtn = this->m_leftArrow;
        auto nextBtn = this->m_rightArrow;

        prevBtn->setVisible(true);
        nextBtn->setVisible(true);

        if (this->m_fields->m_currentPage <= 0) {
            prevBtn->setVisible(false);
        } else if (this->m_fields->m_currentPage >= (BrType::LevelID.size() / 10)) {
            nextBtn->setVisible(false);
        }
        this->m_pageBtn->setVisible(true);
        std::string islegacy = "";
        if ((this->m_fields->m_currentPage) * 10 +1 > 250) {
            islegacy = " (Legacy)";
        }
         if (level_map.empty()) { 
            getlistjson([=](matjson::Value response) {
					int order = 0;
					for (const auto& item : response.asArray().unwrap()) {
						order+=1;
						int curord = order;
						getleveljson(item.asString().unwrap(), [=](matjson::Value response) {
							level_map[curord] = response;
						});
					}
			},[=]() {
                FLAlertLayer::create("Server Error","The server is unable to be reached!","OK")->show();
            });
        } if (BrType::LevelID.size() < 1 && !level_map.empty()) {
            BrType::parseRequestString(level_map);
        }

        this->m_countText->setString((fmt::format("Showing levels {}-{}{} out of {}",clamp((this->m_fields->m_currentPage) * 10+1,0,BrType::LevelID.size()),clamp((this->m_fields->m_currentPage+1) * 10, 0,BrType::LevelID.size() ),islegacy ,BrType::LevelID.size() ).c_str()));
        this->m_pageText->setString(fmt::format("{}",this->m_fields->m_currentPage+1).c_str());
    }
    void updatePageLabel() {
            LevelBrowserLayer::updatePageLabel();
            updText();
            if (!this->m_fields->isBrainrot) {
                return;
            }
            if (this->m_searchObject->m_searchType != SearchType::Type19 && this->m_searchObject->m_searchType  != SearchType::MapPack) {
                return;
            }
            if (this->m_pageBtn) {
                this->m_pageBtn->setVisible(true);
            }
        }
};

