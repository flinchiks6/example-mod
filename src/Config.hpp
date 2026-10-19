#pragma once
#include <Geode/Geode.hpp>
using namespace geode::prelude;

namespace cfg {
    inline bool get(char const* key, bool def = false) {
        return Mod::get()->getSavedValue<bool>(key, def);
    }
    inline void set(char const* key, bool v) {
        Mod::get()->setSavedValue<bool>(key, v);
    }
    inline float getF(char const* key, float def = 1.f) {
        return Mod::get()->getSavedValue<float>(key, def);
    }
    inline void setF(char const* key, float v) {
        Mod::get()->setSavedValue<float>(key, v);
    }

    inline float speed() {
        return get("speed.on") ? getF("speed.val", 1.f) : 1.f;
    }

    // Применяет скорость только во время игры на уровне
    inline void apply() {
        float ts = PlayLayer::get() ? speed() : 1.f;
        CCDirector::get()->getScheduler()->setTimeScale(ts);
    }
}
