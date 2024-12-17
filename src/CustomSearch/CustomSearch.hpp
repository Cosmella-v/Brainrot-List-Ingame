#pragma once
#include <Geode/Bindings.hpp>
#include <Geode/modify/DemonFilterSelectLayer.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/Geode.hpp>
#include "BrType.hpp"
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/chrono.h>
#include <sstream>
#include <time.h>
#include <chrono>

#ifndef GEODE_IS_ANDROID
#include <sys/timeb.h>
#endif

using namespace std::chrono;

using namespace geode::prelude;
static void change_scene() {
        if (!BrType::LoadedAllLevels) {
            BrType::parseRequestString(level_map);
            BrType::filterType = -1;
        }
        BrType::isSearchingBR = true;
        auto browserLayer = LevelBrowserLayer::scene(BrType::getSearchObject(10, 0));
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

    struct Fields {
        int m_currentPage = 0;
        int m_furthestLoadedPage = 0;
        int m_lowIdx = 0;
        bool isBrainrot = false;
        int m_stopratelimit = 0;
    };
    bool init(GJSearchObject* p0) {
        this->m_fields->isBrainrot = BrType::isSearchingBR;
        if (!this->m_fields->isBrainrot) {
            return LevelBrowserLayer::init(p0);
        }

        if (p0->m_searchType != SearchType::Type19) {
            return LevelBrowserLayer::init(p0);
        }
        BrType::isSearchingBR = false;

        this->m_fields->m_currentPage = 0;
        int page = this->m_fields->m_currentPage;
        this->m_fields->m_lowIdx = page * 10;
        this->setUserObject("brl_modified", CCBool::create(true));
        LevelBrowserLayer::init(BrType::getSearchObject(10, 0));
        return true;
    }

    void loadLevelsFinished(cocos2d::CCArray* p0, char const* p1, int p2) {
        LevelBrowserLayer::loadLevelsFinished(p0, p1, p2);
        if (!this->m_fields->isBrainrot) {
            return;
        }
        if (this->m_searchObject->m_searchType != SearchType::Type19) {
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
        if (this->m_searchObject->m_searchType != SearchType::Type19) {
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
        if (this->m_searchObject->m_searchType != SearchType::Type19) {
            return LevelBrowserLayer::onGoToPage(sender);
        }
        SetIDPopup* popup = SetIDPopup::create(this->m_fields->m_currentPage+1,1,(BrType::LevelID.size() / 10)+1,"Go to Page","OK",true,1,60,false,false);
        popup->m_delegate = this;
        popup->show();
    }

    void onPrevPage(CCObject* sender) {
        if (!this->m_fields->isBrainrot) {
            return LevelBrowserLayer::onPrevPage(sender);
        }
        if (this->m_searchObject->m_searchType != SearchType::Type19) {
            return LevelBrowserLayer::onPrevPage(sender);
        }
        if (this->m_fields->m_currentPage > 0) {
            this->m_fields->m_currentPage -= 1;
        }
        nextBtnActions();
        
    }
    void fixtimeout(auto h) {
        LevelBrowserLayer::loadPage(BrType::getSearchObject( ((BrType::LevelID.size() - 10 - this->m_fields->m_currentPage * 10) - BrType::LevelID.size()) *-1, (((BrType::LevelID.size()) - this->m_fields->m_currentPage * 10) - BrType::LevelID.size()) *-1 ));
        this->m_pageBtn->setVisible(true);
        if (CCNode* first_Betterinfo = this->getChildByIDRecursive("cvolton.betterinfo/first-button")) {
            first_Betterinfo->setVisible(this->m_fields->m_currentPage != 0);
        }
    }
    void nextBtnActions() {
        hideStuff();
        auto time = 0.85 - (getFullDoubleTime() - this->m_fields->m_stopratelimit);
        if(time < 0) {
            time = 0;
            fixtimeout(nullptr);
        } else {
            if (CCNode* CFix = this->m_list->m_listView) {
                CFix->setVisible(false);
            }
            this->m_circle->setVisible(true);
            this->getScheduler()->scheduleSelector((cocos2d::SEL_SCHEDULE)(&BRlist::fixtimeout), this, 1, 0, time, false);
        }
        this->m_fields->m_stopratelimit = getFullDoubleTime() + time;
    }

    void hideStuff() {
        if (CCNode* last_Betterinfo = this->getChildByIDRecursive("cvolton.betterinfo/last-button")) {
            typeinfo_cast<CCMenuItemSpriteExtra*>(last_Betterinfo)->m_pfnSelector = (cocos2d::SEL_MenuHandler)(&BRlist::lastpage);
        }
        if (CCNode* filter_Betterinfo = this->getChildByIDRecursive("cvolton.betterinfo/filter-button")) {
            filter_Betterinfo->setVisible(false);
        }
        if (CCNode* first_Betterinfo = this->getChildByIDRecursive("cvolton.betterinfo/first-button")) {
            typeinfo_cast<CCMenuItemSpriteExtra*>(first_Betterinfo)->m_pfnSelector = (cocos2d::SEL_MenuHandler)(&BRlist::firstpage);
            first_Betterinfo->setVisible(this->m_fields->m_currentPage != 0);
        }
        this->m_pageBtn->setVisible(true);
        std::string islegacy = "";
        if ((this->m_fields->m_currentPage) * 10 +1 > 250) {
            islegacy = " (Legacy)";
        }
        this->m_countText->setString((fmt::format("Showing levels {}-{}{} out of {}",(this->m_fields->m_currentPage) * 10+1,clamp((this->m_fields->m_currentPage+1) * 10, 0,BrType::LevelID.size() ),islegacy ,BrType::LevelID.size() ).c_str()));
        this->m_pageText->setString(fmt::format("{}",this->m_fields->m_currentPage+1).c_str());
    }
    void updatePageLabel() {
        LevelBrowserLayer::updatePageLabel();
        if (!this->m_fields->isBrainrot) {
            return;
        }
        if (this->m_searchObject->m_searchType != SearchType::Type19) {
            return;
        }
        if (this->m_pageBtn) {
            this->m_pageBtn->setVisible(true);
        }
    }
};

