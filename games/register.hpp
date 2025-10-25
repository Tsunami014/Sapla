#pragma once
// Some includes for game plugins to use
#include "../main.hpp"
#include "../log.hpp"
#include "../menu.hpp"
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
    extern "C" bool initWrapper() { return initFn(); }\
    extern "C" bool (*reg())() { return &initWrapper; }\
    extern "C" const Version version() { return { versionFrom, versionTo }; }

