#pragma once
#include <Geode/Geode.hpp>
#include <Geode/Bindings.hpp>

#include <Geode/modify/DemonFilterSelectLayer.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/modify/GJSearchObject.hpp>
#include <Geode/modify/MapPackCell.hpp>

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/chrono.h>
#include <sstream>
#include <time.h>
#include <chrono>

#include "BrType.hpp"
#include "Packlist.hpp"

#include <Geode/utils/NodeIDs.hpp>

#ifndef GEODE_IS_ANDROID
#include <sys/timeb.h>
#endif

double timeBD = 0;
double m_stopratelimit = 0;

using namespace geode::prelude;
using namespace geode::node_ids;
using namespace std::chrono;

class $modify(FixMapPackCell, MapPackCell) {
    static void onModify(auto& self) {
        (void)self.setHookPriority("MapPackCell::loadFromMapPack", -3998); 
    }
    struct Fields {
        bool m_modifiedByBRL = false;
    };
    void loadFromMapPack(GJMapPack* cell) {
        MapPackCell::loadFromMapPack(cell);
        if (cell->getUserObject("brl_modified"_spr)) {
            this->m_fields->m_modifiedByBRL = true;
            this->m_viewButton->m_pfnSelector = menu_selector(FixMapPackCell::onBRLMapPack);

            // no fucking node ids and rob fucked up the code
            CCSprite* spr = this->getChildByType<CCSprite>(0);
            if (spr) {
                cocos2d::CCPoint difficultyPos = spr->getPosition() + CCPoint { .0f, .0f };
                for (auto item : CCArrayExt<CCNode*>(this->getChildren())) {
                    auto sprite = typeinfo_cast<CCSprite*>(item);
                    if (!sprite || sprite->getChildByID("brl-map-pack-sprite"_spr)) continue;
                    item->setOpacity(0);
                }
                CCNode* mdSpr = this->getChildByID("brl-map-pack-sprite"_spr);
                if (!mdSpr) {
                    mdSpr = (!Mod::get()->getSettingValue<bool>("better-face")) ?
                    CCSprite::create("normal_face_with_demon_text.png"_spr) : CCSprite::create("Betterface.png"_spr);
                }
                mdSpr->setPosition(difficultyPos);
                if (mdSpr->getParent() != this) this->addChild(mdSpr);
                mdSpr->setZOrder(spr->getZOrder());
                mdSpr->setScale((!Mod::get()->getSettingValue<bool>("better-face")) ? 0.2 : 0.4);
                mdSpr->setID("brl-map-pack-sprite"_spr);
            }
        }
        return;
    };

    void onBRLMapPack(CCObject*) {
        // log::debug("Should load new scene");
        BrType::ShouldChangeText = this->m_mapPack->m_packName;
        auto browserLayer = LevelBrowserLayer::scene(GJSearchObject::create(SearchType::Type19, this->m_mapPack->m_levelStrings));
        CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, browserLayer));
    }

};

static void switchToBRLScene(bool mappack = false,bool init = true) {
        if (!BrType::LoadedAllLevels) {
            BrType::parseRequestString(level_map);
            BrType::filterType = -1;
        }
        if (mappack) {
            BrType::MapPack_Br = true;
            BrType::isSearchingBR = false;
        } else {
            BrType::isSearchingBR = true;
            BrType::MapPack_Br = false;
        }
        if (init) {
            auto browserLayer = LevelBrowserLayer::scene(BrType::getSearchObject(10, 0,false));
            CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, browserLayer));
        } else {
            auto browserLayer =LevelBrowserLayer::create(BrType::getSearchObject(10, 0,false));
            geode::cocos::switchToScene(browserLayer);
        }
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


class $modify(BRList, LevelBrowserLayer) {
    static void onModify(auto& self) {
        (void) self.setHookPriority("LevelBrowserLayer::init", -3998); 
    }
    struct Fields {
        int m_currentPage = 0;
        int m_furthestLoadedPage = 0;
        int m_lowIdx = 0;
        bool isBrainrot = false;
        bool MapPack_Br = false;
        gd::string mapPackText;
    };

    void switchThing(CCObject*) {
        switchToBRLScene(this->m_fields->isBrainrot,false);
    }

    CCNode* CreateButtonSprite() {
        CCNode* returns;
         if (this->m_fields->MapPack_Br) {
             returns = CCMenuItemSpriteExtra::create(
                CircleButtonSprite::createWithSprite(
                    "list_icon.png"_spr, 1.1,
                    (Mod::get()->getSettingValue<bool>("dark-mode")) ? CircleBaseColor::DarkPurple : CircleBaseColor::Green,
                    CircleBaseSize::Small
                ),
                this,
                menu_selector(BRList::switchThing)
            );
        } else {
            returns = CCMenuItemSpriteExtra::create(
                CircleButtonSprite::createWithSprite(
                    "mapPack.png"_spr, 0.95,
                    (Mod::get()->getSettingValue<bool>("dark-mode")) ? CircleBaseColor::DarkPurple : CircleBaseColor::Green,
                    CircleBaseSize::Small
                ),
                this,
                menu_selector(BRList::switchThing)
            );
         };

        return returns;
    }
    void createButton() {
         if (CCNode* spr = CreateButtonSprite()) {
            CCMenu* menu = CCMenu::create();
            auto winSize = CCDirector::get()->getWinSize();
            menu->setLayout(
						ColumnLayout::create()
							->setAxisReverse(true)
							->setGap(10)
							->setAxisAlignment(AxisAlignment::End)
					);
			menu->setContentSize({menu->getContentSize().width, winSize.height-10});
            menu->setID("brl-extra"_spr);
            menu->setPosition({63,260});
            if (CCNode* Child = this->getChildByIDRecursive("back-menu")) {
                menu->setPosition(Child->getPosition() - ccp(0.f,Child->getContentHeight()));
            }
            menu->setAnchorPoint({1.3,0.950}); 
            this->addChild(menu);
            menu->addChild(spr);
            menu->updateLayout();
        }

    }
    bool init(GJSearchObject* p0) {
        this->m_fields->isBrainrot = BrType::isSearchingBR;
        this->m_fields->mapPackText = BrType::ShouldChangeText;
        this->m_fields->MapPack_Br = BrType::MapPack_Br;
        BrType::MapPack_Br = false;
        BrType::ShouldChangeText = "";
        if (this->m_fields->MapPack_Br) {
            this->m_fields->m_currentPage = 0;
            int page = this->m_fields->m_currentPage;
            this->m_fields->m_lowIdx = page * 10;
            bool inits = LevelBrowserLayer::init(BrType::getSearchObject(10, 0, this->m_fields->MapPack_Br));
            updateText();
            createButton();
            return inits;
        }
        if (!this->m_fields->isBrainrot) {
            bool inits = LevelBrowserLayer::init(p0);
            updateText();
            return inits;
        }

        if (p0->m_searchType != SearchType::Type19 && p0->m_searchType != SearchType::MapPack) {
            return LevelBrowserLayer::init(p0);
        }
        BrType::isSearchingBR = false;
        
        this->m_fields->m_currentPage = 0;
        int page = this->m_fields->m_currentPage;
        this->m_fields->m_lowIdx = page * 10;
        this->setUserObject("brl_modified"_spr, CCBool::create(true));
        LevelBrowserLayer::init(BrType::getSearchObject(10, 0, this->m_fields->MapPack_Br));
        createButton();
        hideStuff();
        return true;
    }
    void updateText() {
        if (!this->m_fields->mapPackText.empty()) {
            if (CCNode* levelList = this->m_list) {
                if (CCLabelBMFont* titleLabel = typeinfo_cast<CCLabelBMFont*>(levelList->getChildByID("title"))) {
                    titleLabel->setString(this->m_fields->mapPackText.c_str());
                };
            }
        }
    }
    void loadLevelsFinished(cocos2d::CCArray* p0, char const* p1, int p2) {
        if (this->m_fields->MapPack_Br && this->m_searchObject->m_searchType == SearchType::MapPack) {
            p0 = BRPacks::GetPacks(this->m_fields->m_currentPage * 10);
        } 
        LevelBrowserLayer::loadLevelsFinished(p0, p1, p2);
        updateText();
        if (!this->m_fields->isBrainrot && !this->m_fields->MapPack_Br) {
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

        int max = 0;
        if (this->m_fields->MapPack_Br) {
            max = BRPacks::maxcount;
        } else {
            max = ((BrType::LevelID.size()-1) / 10);
        }

        if (this->m_fields->m_currentPage <= 0) {
            prevBtn->setVisible(false);
        }
        if (this->m_fields->m_currentPage >= max) {
            nextBtn->setVisible(false);
        }
    }
    void lastPage(auto h) {
        int max = 0;
        if (this->m_fields->MapPack_Br) {
            max = BRPacks::maxcount;
        } else {
            max = ((BrType::LevelID.size()-1) / 10);
        }
        if (this->m_fields->m_currentPage == max) {
            return;
        };
        this->m_fields->m_currentPage = max;
        nextBtnActions();
    }

    void firstPage(auto h) {
         if (this->m_fields->m_currentPage == 0) {
            return;
        };
        this->m_fields->m_currentPage = 0;
        nextBtnActions();
    }

    void onNextPage(CCObject* sender) {
        updateText();
        if (!this->m_fields->isBrainrot && !this->m_fields->MapPack_Br) {
            return LevelBrowserLayer::onNextPage(sender);
        }
        if (this->m_searchObject->m_searchType != SearchType::Type19  && this->m_searchObject->m_searchType != SearchType::MapPack) {
            return LevelBrowserLayer::onNextPage(sender);
        }
        int max = 0;
        if (this->m_fields->MapPack_Br) {
            max = BRPacks::maxcount;
        } else {
            max = ((BrType::LevelID.size()-1) / 10);
        }
        if (this->m_fields->m_currentPage < max) {
            this->m_fields->m_currentPage += 1;
        }
        nextBtnActions();
        
    }
    void setIDPopupClosed(SetIDPopup* popup, int p1) {
        if (!this->m_fields->isBrainrot && !this->m_fields->MapPack_Br) {
            return LevelBrowserLayer::setIDPopupClosed(popup,p1);
        }
        if (this->m_searchObject->m_searchType != SearchType::Type19 && this->m_searchObject->m_searchType != SearchType::MapPack) {
            return LevelBrowserLayer::setIDPopupClosed(popup,p1);
        }
        p1-=1;

        int max = 0;
        if (this->m_fields->MapPack_Br) {
            max = BRPacks::maxcount;
        } else {
            max = (BrType::LevelID.size() / 10);
        }

        if (this->m_fields->m_currentPage == p1) {
            return;
        }
        if (p1 >= max) {
           return lastPage(nullptr);
        };
        this->m_fields->m_currentPage = p1;

        nextBtnActions();
    }
    void onGoToPage(CCObject* sender) {
        if (!this->m_fields->isBrainrot && !this->m_fields->MapPack_Br) {
            return LevelBrowserLayer::onGoToPage(sender);
        }
        if (this->m_searchObject->m_searchType != SearchType::Type19 && this->m_searchObject->m_searchType  != SearchType::MapPack) {
            return LevelBrowserLayer::onGoToPage(sender);
        }
        int max = 0;
        if (this->m_fields->MapPack_Br) {
            max = BRPacks::maxcount;
        } else {
            max = (BrType::LevelID.size() / 10);
        }

        SetIDPopup* popup = SetIDPopup::create(this->m_fields->m_currentPage + 1, 1, (max), "Go to Page", "OK", true, 1, 60, false, false);
        popup->m_delegate = this;
        popup->show();
    }

    void onPrevPage(CCObject* sender) {
        updateText();
        
        if (!this->m_fields->isBrainrot && !this->m_fields->MapPack_Br) {
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
        updateText();
        return;
    }
    void fixTimeout(auto h) {
        m_stopratelimit = getFullDoubleTime() + timeBD;
        BRList::loadPage(BrType::getSearchObject( ((BrType::LevelID.size() - 10 - this->m_fields->m_currentPage * 10) - BrType::LevelID.size()) * -1, (((BrType::LevelID.size()) - this->m_fields->m_currentPage * 10) - BrType::LevelID.size()) * -1,this->m_fields->MapPack_Br ));
        this->m_pageBtn->setVisible(true);
        if (CCNode* betterInfoFirstPage = this->getChildByIDRecursive("cvolton.betterinfo/first-button")) {
            betterInfoFirstPage->setVisible(this->m_fields->m_currentPage != 0);
        }
        hideStuff();
    }
    void nextBtnActions() {
        hideStuff();
        timeBD = 0.85 - (getFullDoubleTime() - m_stopratelimit);
        if (timeBD < 0) {
            timeBD = 0;
            fixTimeout(nullptr);
        } else {
            if (CCNode* CFix = this->m_list->m_listView) {
                CFix->setVisible(false);
            }
            this->m_circle->setVisible(true);
            this->getScheduler()->scheduleSelector(schedule_selector(BRList::fixTimeout), this, 1, 0, timeBD, false);
        }
    }
    void hideStuff() {
        int max = 0;
        int largesize = 0;
        if (this->m_fields->MapPack_Br) {
            max = BRPacks::maxcount;
            largesize = BRPacks::realmaxcount;
        } else {
            max = (BrType::LevelID.size() / 10);
            largesize = BrType::LevelID.size();
        }

        if (auto betterInfoLastPage = typeinfo_cast<CCMenuItemSpriteExtra*>(this->getChildByIDRecursive("cvolton.betterinfo/last-button"))) {
            betterInfoLastPage->m_pfnSelector = menu_selector(BRList::lastPage);
            betterInfoLastPage->setVisible(this->m_fields->m_currentPage < max);
        }
        if (auto betterInfoFilter = this->getChildByIDRecursive("cvolton.betterinfo/filter-button")) {
            betterInfoFilter->setVisible(false);
        }
        if (auto betterInfoFirstPage = typeinfo_cast<CCMenuItemSpriteExtra*>(this->getChildByIDRecursive("cvolton.betterinfo/first-button"))) {
            betterInfoFirstPage->m_pfnSelector = menu_selector(BRList::firstPage);
            betterInfoFirstPage->setVisible(this->m_fields->m_currentPage != 0);
        }
        auto prevBtn = this->m_leftArrow;
        auto nextBtn = this->m_rightArrow;

        prevBtn->setVisible(true);
        nextBtn->setVisible(true);

        if (this->m_fields->m_currentPage <= 0) {
            prevBtn->setVisible(false);
        } 
        if (this->m_fields->m_currentPage >= max) {
            nextBtn->setVisible(false);
        }
            this->m_pageBtn->setVisible(true);
            std::string islegacy = "";
            if (this->m_fields->isBrainrot) {
                if ((this->m_fields->m_currentPage) * 10 +1 > 250) {
                    islegacy = " (Legacy)";
                }
            }
            BrType::shownServerError = false;
            if (level_map.empty()) { 
                getlistjson([=](matjson::Value response) {
                    int order = 0;
                    for (const auto& item : response.asArray().unwrap()) {
                        order += 1;
                        int curord = order;
                        getleveljson(item.asString().unwrap(), [=](matjson::Value response) {
                            level_map[curord] = response;
                        });
                    }
                },[=]() {
                    if (BrType::shownServerError) {
                        return;
                    }
                    BrType::shownServerError = true;
                    FLAlertLayer::create("Server Error", "The server is unable to be reached!", "OK")->show();
                });
            } if (BrType::LevelID.size() < 1 && !level_map.empty()) {
                BrType::parseRequestString(level_map);
            }

            if (BRPacks::PacksIDs.empty()) {
                getpackjson([=](matjson::Value response) {
                    for (auto item : response.asArray().unwrap()) {
                        log::debug("{}", item.dump());
                        for (const auto& lels : item["levels"].asArray().unwrap()) {
                            getleveljson(lels.asString().unwrap(), [=](matjson::Value response) {
                                BRPacks::level_map[lels.asString().unwrap()] = response;
                                BRPacks::levelid_map[response["id"].asInt().unwrap()] = response;
                            });
                        }
                        BRPacks::PacksIDs.emplace_back(item["name"].asString().unwrap(),item["levels"]);
                    }
                },[=]() {
                    if (BrType::shownServerError) {
                        return;
                    }
                    BrType::shownServerError = true;
                    FLAlertLayer::create("Server Error","The server is unable to be reached!","OK")->show();
                });
            }
        this->m_countText->setString((fmt::format("{} to {}{} of {}",clamp((this->m_fields->m_currentPage) * 10+1,0,largesize),clamp((this->m_fields->m_currentPage+1) * 10, 0,largesize),islegacy ,largesize ).c_str()));
        this->m_pageText->setString(fmt::format("{}",this->m_fields->m_currentPage+1).c_str());
    }
    void updatePageLabel() {
            LevelBrowserLayer::updatePageLabel();
            updateText();
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

