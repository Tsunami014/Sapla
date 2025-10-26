#pragma once
#include "link.hpp"
// Some includes for game plugins to use
#include "../main.hpp"
#include "../log.hpp"
#include "../menu.hpp"
// Base
#include "../items/base/svgRend.hpp"
#include "../items/base/svgItem.hpp"
#include "../items/base/svgBtnItem.hpp"
#include "../items/base/font.hpp"
#include "../items/base/markdown.hpp"
// UI
#include "../items/progress.hpp"
#include "../items/treeItem.hpp"
// Cards stuff
#include "../cards/getCards.hpp"
#include "../cards/cardTyps.hpp"
#include "../items/sideRends.hpp"
// Scenes
#include "../scenes/baseScn.hpp"
#include "../game.hpp"  // to mitigate the forward reference
#include "../scenes/homeScn.hpp"

inline Registry makeRegistry(
    int vFrom,
    int vTo,
    bool (*onRun)(),
    void (*onLoad)() = +[](){},
    void (*onUnload)() = +[](){}
) { return Registry{onLoad, onRun, onUnload, vFrom, vTo}; }

#define REGISTER_GAME(reg) \
    extern "C" Registry _register() { return reg; }

