#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include "ModMenu.hpp"
#include "Config.hpp"

using namespace geode::prelude;

static CCMenuItemSpriteExtra* makeButton(CCObject* target, SEL_MenuHandler cb) {
    auto spr = CircleButtonSprite::create(
        CCLabelBMFont::create("M", "bigFont.fnt"),
        CircleBaseColor::Green,
        CircleBaseSize::Medium
    );
    auto btn = CCMenuItemSpriteExtra::create(spr, target, cb);
    btn->setID("mod-menu-button"_spr);
    return btn;
}

// Кнопка в главном меню
class $modify(MMMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        if (auto menu = this->getChildByID("bottom-menu")) {
            menu->addChild(makeButton(this, menu_selector(MMMenuLayer::onModMenu)));
            menu->updateLayout();
        }
        return true;
    }
    void onModMenu(CCObject*) { ModMenu::create()->show(); }
};

// Кнопка в паузе
class $modify(MMPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();
        if (auto menu = this->getChildByID("right-button-menu")) {
            menu->addChild(makeButton(this, menu_selector(MMPauseLayer::onModMenu)));
            menu->updateLayout();
        }
    }
    void onModMenu(CCObject*) { ModMenu::create()->show(); }
};

// Логика читов
class $modify(MMPlayLayer, PlayLayer) {
    struct Fields {
        bool cheated = false;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
        cfg::apply();
        return true;
    }

    void onQuit() {
        PlayLayer::onQuit();
        CCDirector::get()->getScheduler()->setTimeScale(1.f);
    }

    void resetLevel() {
        m_fields->cheated = false;
        PlayLayer::resetLevel();
    }

    void postUpdate(float dt) {
        if (cfg::speed() != 1.f) m_fields->cheated = true;
        PlayLayer::postUpdate(dt);
    }

    void destroyPlayer(PlayerObject* player, GameObject* object) {
        if (cfg::get("noclip")) {
            m_fields->cheated = true;
            return;
        }
        PlayLayer::destroyPlayer(player, object);
    }

    void levelComplete() {
        if (m_fields->cheated && cfg::get("safe", true)) {
            Notification::create("Safe Mode: completion blocked", NotificationIcon::Warning)->show();
            this->resetLevel();
            return;
        }
        PlayLayer::levelComplete();
    }
};
