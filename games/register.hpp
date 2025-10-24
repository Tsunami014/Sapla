#pragma once
#include "link.hpp"
// Some includes for game plugins to use
#include "../log.hpp"
// Base
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

extern "C" struct Version {
    int vFrom;
    int vTo;
};

#define REGISTER_GAME(initFn, versionFrom, versionTo) \
    extern "C" bool initWrapper(const Link* ln) { return initFn(ln); }\
    extern "C" bool (*reg())(const Link*) { return &initWrapper; }\
    extern "C" const Version version() { return { versionFrom, versionTo }; }

