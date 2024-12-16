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
        if (BrType::firstTimeOpen) {
                BrType::parseRequestString(level_map);
                BrType::firstTimeOpen = false;
                BrType::filterType = -1;
        }
        BrType::isSearchingBR = true;
        auto browserLayer = LevelBrowserLayer::create(BrType::getSearchObject(10, 0));
        geode::cocos::switchToScene(browserLayer);
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
        if (!BrType::isSearchingBR) {
            LevelBrowserLayer::init(p0);
            return true;
        }

        if (p0->m_searchType != SearchType::Type19) {
            LevelBrowserLayer::init(p0);
            return true;
        }
        BrType::isSearchingBR = false;

        this->m_fields->m_currentPage = 0;
        int page = this->m_fields->m_currentPage;
        this->m_fields->m_lowIdx = page * 10;

        LevelBrowserLayer::init(BrType::getSearchObject(10, 0));
        this->m_pageBtn->setVisible(false);
        if (CCNode* Search = this->getChildByIDRecursive("search-menu")) {
            Search->setVisible(false);
        }
        return true;
    }

    // TodoReturn loadPage(GJSearchObject* p0) {

    //     if (!ListManager::isSupremeSearching) {
    //         LevelBrowserLayer::loadPage(p0);
    //         return;
    //     }

    //     LevelBrowserLayer::loadPage(p0);
    //     return;
        
    // }

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

        if (this->m_fields->m_currentPage <= 0) {
            prevBtn->setVisible(false);
        } else if (this->m_fields->m_currentPage >= (BrType::LevelID.size() / 10)) {
            nextBtn->setVisible(false);
        }
    }

    void onNextPage(CCObject* sender) {
        LevelBrowserLayer::onNextPage(sender);
        if (!this->m_fields->isBrainrot) {
            return;
        }
        if (this->m_searchObject->m_searchType != SearchType::Type19) {
            return;
        }

        if (this->m_fields->m_currentPage < (BrType::LevelID.size() / 10)) {
            this->m_fields->m_currentPage += 1;
        }
        nextBtnActions();
        
    }

    void onPrevPage(CCObject* sender) {
        LevelBrowserLayer::onPrevPage(sender);
        if (!this->m_fields->isBrainrot) {
            return;
        }
        if (this->m_searchObject->m_searchType != SearchType::Type19) {
            return;
        }
        if (this->m_fields->m_currentPage > 0) {
            this->m_fields->m_currentPage -= 1;
        }
        nextBtnActions();
        
    }

    void fixtimeout(auto h) {
        LevelBrowserLayer::loadPage(BrType::getSearchObject( ((BrType::LevelID.size() - 10 - this->m_fields->m_currentPage * 10) - BrType::LevelID.size()) *-1, (((BrType::LevelID.size()) - this->m_fields->m_currentPage * 10) - BrType::LevelID.size()) *-1 ));
        this->m_pageBtn->setVisible(false);
    }
    void nextBtnActions() {
        hideStuff();
        auto time = 0.75 - (getFullDoubleTime() - this->m_fields->m_stopratelimit);
        if(time < 0) {
            time = 0;
            fixtimeout(nullptr);
        } else {
            if (CCNode* CFix = this->m_list->getChildByID("list-view")) {
                CFix->setVisible(false);
            }
            this->m_circle->setVisible(true);
            this->getScheduler()->scheduleSelector(schedule_selector(BRlist::fixtimeout), this, 1, 0, time, false);
        }
        this->m_fields->m_stopratelimit = getFullDoubleTime() + time;
    }

    void hideStuff() {
        if (CCNode* Search = this->getChildByIDRecursive("search-menu")) {
            Search->setVisible(false);
        }
        std::string islegacy = "";
        if ((this->m_fields->m_currentPage) * 10 +1 > 250) {
            islegacy = " (Legacy)";
        }
        this->m_countText->setString((fmt::format("Showing levels {}-{}{} out of {}",(this->m_fields->m_currentPage) * 10+1,clamp((this->m_fields->m_currentPage+1) * 10, 0,BrType::LevelID.size() ),islegacy ,BrType::LevelID.size() ).c_str()));
    }
};

