#pragma once
#include <Geode/Geode.hpp>
using namespace geode::prelude;

class ModMenu : public Popup<> {
protected:
    CCMenu* m_tabMenu = nullptr;
    CCNode* m_content = nullptr;
    CCLabelBMFont* m_speedLabel = nullptr;
    int m_tab = 0;

    bool setup() override;
    void showTab(int tab);
    void addToggle(CCMenu* menu, char const* label, char const* key, bool def, float y);

    void onTab(CCObject* sender);
    void onToggle(CCObject* sender);
    void onSpeed(CCObject* sender);

public:
    static ModMenu* create();
};
