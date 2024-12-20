#pragma once
#include <Geode/Geode.hpp>
#include <Geode/Bindings.hpp>

using namespace geode::prelude;
class BRPacks {
public:
    inline static cocos2d::CCArray* GetPacks() {
    cocos2d::CCArray* Newarrow = new cocos2d::CCArray;
    CCDictionary* packdata = CCDictionary::create();
        
        packdata->setObject(CCString::create("1"), "1");
        packdata->setObject(CCString::create("Fuck robtop's code Pack"), "2");
                packdata->setObject(CCString::create("128,100000"), "3");
                packdata->setObject(CCString::create("0"), "4");
                packdata->setObject(CCString::create("0"), "5");
                packdata->setObject(CCString::create("6"), "6");
                packdata->setObject(CCString::create("255,255,255"), "7");
                packdata->setObject(CCString::create("255,255,255"), "8");

                GJMapPack* pack1 = GJMapPack::create(packdata);
                pack1->setUserObject("brl_modified",CCBool::create(true));
                Newarrow->addObjectNew(pack1);
                return Newarrow;
    };
};
