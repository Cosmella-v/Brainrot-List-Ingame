#pragma once
#include <Geode/Bindings.hpp>
#include <Geode/modify/DemonFilterSelectLayer.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/Geode.hpp>
#include "BrType.hpp"
using namespace geode::prelude;
static void change_scene() {
        if (BrType::firstTimeOpen) {
                BrType::parseRequestString(level_map);
                BrType::firstTimeOpen = false;
                BrType::filterType = -1;
        }
        BrType::isSearchingBR = true;
        auto browserLayer = LevelBrowserLayer::create(BrType::getSearchObject(24, 0));
        geode::cocos::switchToScene(browserLayer);
}

#include <Geode/Bindings.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>

class $modify(GrDLevelBrowserLayer, LevelBrowserLayer) {

    struct Fields {
        int m_currentPage = 0;
        int m_furthestLoadedPage = 0;
        int m_lowIdx = 0;
    };

    bool init(GJSearchObject* p0) {

        if (!BrType::isSearchingBR) {
            LevelBrowserLayer::init(p0);
            return true;
        }

        if (p0->m_searchType != SearchType::Type19) {
            LevelBrowserLayer::init(p0);
            return true;
        }

        this->m_fields->m_currentPage = 0;
        int page = this->m_fields->m_currentPage;
        this->m_fields->m_lowIdx = page * 10;

        LevelBrowserLayer::init(BrType::getSearchObject(24, 0));
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
        if (!BrType::isSearchingBR) {
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
        } else if (this->m_fields->m_currentPage >= 24) {
            nextBtn->setVisible(false);
        }
    }

    void onNextPage(CCObject* sender) {
        LevelBrowserLayer::onNextPage(sender);
        if (!BrType::isSearchingBR) {
            return;
        }
        if (this->m_searchObject->m_searchType != SearchType::Type19) {
            return;
        }

        if (this->m_fields->m_currentPage < 24) {
            this->m_fields->m_currentPage += 1;
        }
        nextBtnActions();
        
    }

    void onPrevPage(CCObject* sender) {
        LevelBrowserLayer::onPrevPage(sender);
        if (!BrType::isSearchingBR) {
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

    void nextBtnActions() {
        hideStuff();
        LevelBrowserLayer::loadPage(BrType::getSearchObject(240 - this->m_fields->m_currentPage * 10, 240 - this->m_fields->m_currentPage * 10));
    }

    void hideStuff() {
        this->m_pageBtn->setVisible(false);
        this->m_countText->setString(fmt::format("{} to {} of 250", 240 - this->m_fields->m_currentPage * 10 + 1, this->m_fields->m_currentPage * 10 + 10).c_str());
    }
};

