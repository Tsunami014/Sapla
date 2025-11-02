#pragma once
#include "link.hpp"
// Some includes for game plugins to use
#include "../core.hpp"
#include "../log.hpp"
#include "../menu.hpp"
// Base
#include "../base/font.hpp"
#include "../base/markdown.hpp"
#include "../base/rectIt.hpp"
#include "../base/svgRend.hpp"
// UI
#include "../items/progress.hpp"
#include "../items/treeItem.hpp"
#include "../wids/svgBtn.hpp"
#include "../wids/svgPic.hpp"
// Cards stuff
#include "../cards/cardTyps.hpp"
#include "../cards/getCards.hpp"
#include "../cards/cardList.hpp"
#include "../items/sideRends.hpp"
// Scenes
#include "../scenes/gameScn.hpp"
#include "../game.hpp"  // to mitigate the forward reference
#include "../scenes/homeScn.hpp"
#include "../scenes/winScn.hpp"

inline Registry makeRegistry(
    int vFrom,
    int vTo,
    bool (*onRun)(),
    void (*onLoad)() = +[](){},
    void (*onUnload)() = +[](){}
) { return Registry{onLoad, onRun, onUnload, vFrom, vTo}; }

#define REGISTER_GAME(reg) \
    extern "C" Registry _register() { return reg; }

