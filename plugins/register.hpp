#pragma once
#include "link.hpp"
// Some includes for plugins to use
#include "../core.hpp"
#include "../log.hpp"
#include "../menu.hpp"
// Base
#include "../base/font.hpp"
#include "../base/markdown.hpp"
#include "../base/rectIt.hpp"
#include "../base/svgRend.hpp"
#include "../base/svgUtils.hpp"
// UI
#include "../items/progress.hpp"
#include "../items/treeItem.hpp"
#include "../wids/svgBtn.hpp"
#include "../wids/svgPic.hpp"
#include "../wids/topbar.hpp"
// Cards stuff
#include "../notes/note.hpp"
#include "../notes/features.hpp"
#include "../notes/getNotes.hpp"
#include "../notes/cardList.hpp"
// Scenes
#include "../scenes/gameScn.hpp"
#include "../game.hpp"  // to mitigate the forward reference
#include "../scenes/homeScn.hpp"
#include "../scenes/winScn.hpp"

#define _makeFns(name, typ) \
    std::vector<typ> name##Fns; \
    RegistryBuilder& add##name(typ fn) { \
        name##Fns.push_back(fn); \
        return *this; \
    } \
    RegistryBuilder& add##name##s(std::vector<typ> fns) { \
        name##Fns.insert(name##Fns.end(), fns.begin(), fns.end()); \
        return *this; \
    }

struct RegistryBuilder {
    _makeFns(Load, VoidFn)
    _makeFns(Unload, VoidFn)
    _makeFns(Play, BoolFn)
    _makeFns(Styl, VoidFn)

    Registry build(const char* desc) {
        return Registry{
            { LoadFns.data(),   int(LoadFns.size()) },
            { UnloadFns.data(), int(UnloadFns.size()) },
            { PlayFns.data(),   int(PlayFns.size()) },
            { StylFns.data(),   int(StylFns.size()) },
            desc,
        };
    }
};
#undef _makeFns

#define REGISTER_PLUG(build, vFrom, vTo) \
    extern "C" Registry _register() { return build; }\
    extern "C" Version _versions() { return  { vFrom, vTo }; }

