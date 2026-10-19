#include "ModMenu.hpp"
#include "Config.hpp"

ModMenu* ModMenu::create() {
    auto ret = new ModMenu();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool ModMenu::init() {
    if (!Popup::init(380.f, 250.f)) return false;

    this->setTitle("Modern Mod Menu");

    // тёмная "стеклянная" панель
    m_bgSprite->setColor({18, 20, 28});
    m_bgSprite->setOpacity(235);

    m_tabMenu = CCMenu::create();
    m_tabMenu->setContentSize({m_size.width - 30.f, 30.f});
    m_tabMenu->setPosition({m_size.width / 2, m_size.height - 52.f});
    m_tabMenu->setLayout(
        RowLayout::create()->setGap(8.f)->setAxisAlignment(AxisAlignment::Center)
    );
    m_mainLayer->addChild(m_tabMenu);

    m_content = CCNode::create();
    m_content->setContentSize(m_size);
    m_content->setAnchorPoint({0.5f, 0.5f});
    m_content->setPosition({m_size.width / 2, m_size.height / 2});
    m_mainLayer->addChild(m_content);

    showTab(0);
    return true;
}

void ModMenu::showTab(int tab) {
    m_tab = tab;
    m_tabMenu->removeAllChildren();
    m_content->removeAllChildren();
    m_speedLabel = nullptr;

    char const* names[] = {"Player", "Info"};
    for (int i = 0; i < 2; i++) {
        auto spr = ButtonSprite::create(
            names[i], 80, true, "bigFont.fnt",
            i == tab ? "GJ_button_01.png" : "GJ_button_04.png", 26.f, 0.55f
        );
        auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ModMenu::onTab));
        btn->setTag(i);
        m_tabMenu->addChild(btn);
    }
    m_tabMenu->updateLayout();

    auto menu = CCMenu::create();
    menu->setPosition({0, 0});
    m_content->addChild(menu);

    float top = m_size.height - 100.f;

    if (tab == 0) {
        addToggle(menu, "Noclip", "noclip", false, top);
        addToggle(menu, "Speedhack", "speed.on", false, top - 36.f);

        float sp = cfg::getF("speed.val", 1.f);
        m_speedLabel = CCLabelBMFont::create(fmt::format("Speed: {:.2f}x", sp).c_str(), "bigFont.fnt");
        m_speedLabel->setScale(0.4f);
        m_speedLabel->setAnchorPoint({0, 0.5f});
        m_speedLabel->setPosition({28.f, top - 66.f});
        m_content->addChild(m_speedLabel);

        auto slider = Slider::create(this, menu_selector(ModMenu::onSpeed), 0.7f);
        slider->setPosition({m_size.width / 2 + 40.f, top - 66.f});
        slider->setValue((sp - 0.25f) / 2.75f);
        m_content->addChild(slider);

        addToggle(menu, "Safe Mode (block completion)", "safe", true, top - 104.f);
    } else {
        auto info = CCLabelBMFont::create(
            "Modern Mod Menu v1.0\n"
            "Safe Mode blocks level completion\n"
            "if cheats were used in the attempt.",
            "chatFont.fnt"
        );
        info->setAlignment(kCCTextAlignmentCenter);
        info->setScale(0.9f);
        info->setPosition({m_size.width / 2, m_size.height / 2 - 20.f});
        m_content->addChild(info);
    }

    // небольшая анимация появления
    m_content->setScale(0.92f);
    m_content->runAction(CCEaseBackOut::create(CCScaleTo::create(0.25f, 1.f)));
}

void ModMenu::addToggle(CCMenu* menu, char const* label, char const* key, bool def, float y) {
    auto lbl = CCLabelBMFont::create(label, "bigFont.fnt");
    lbl->setScale(0.45f);
    lbl->setAnchorPoint({0, 0.5f});
    lbl->setPosition({28.f, y});
    m_content->addChild(lbl);

    auto toggler = CCMenuItemToggler::createWithStandardSprites(
        this, menu_selector(ModMenu::onToggle), 0.7f
    );
    toggler->toggle(cfg::get(key, def));
    toggler->setPosition({m_size.width - 40.f, y});
    toggler->setUserObject("key", CCString::create(key));
    menu->addChild(toggler);
}

void ModMenu::onTab(CCObject* sender) {
    int t = static_cast<CCNode*>(sender)->getTag();
    if (t == m_tab) return;
    // откладываем перестройку, чтобы не удалять кнопку внутри её же колбэка
    Loader::get()->queueInMainThread([self = Ref(this), t] {
        self->showTab(t);
    });
}

void ModMenu::onToggle(CCObject* sender) {
    auto t = static_cast<CCMenuItemToggler*>(sender);
    auto key = static_cast<CCString*>(t->getUserObject("key"))->getCString();
    // колбэк вызывается до смены состояния
    cfg::set(key, !t->isToggled());
    cfg::apply();
}

void ModMenu::onSpeed(CCObject* sender) {
    float v = static_cast<SliderThumb*>(sender)->getValue();
    float sp = std::round((0.25f + v * 2.75f) * 20.f) / 20.f;
    cfg::setF("speed.val", sp);
    if (m_speedLabel) {
        m_speedLabel->setString(fmt::format("Speed: {:.2f}x", sp).c_str());
    }
    cfg::apply();
}
