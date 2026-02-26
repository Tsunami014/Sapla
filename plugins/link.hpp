#pragma once
#include <vector>

// NOTE: If there are strange errors PLEASE ensure the order of the Registry aligns with the order in the RegistryBuilder

extern "C" {
    typedef void (*VoidFn)();
    typedef bool (*BoolFn)();

    struct VFnArray {
        VoidFn* fns;
        int count;
    };
    struct BFnArray {
        BoolFn* fns;
        int count;
    };

    struct Registry {
        VFnArray onLoad;
        VFnArray onLoadFeats;
        VFnArray onUnload;
        BFnArray onPlay;
        VFnArray onStyl;

        const char* desc;
    };

    struct Version {
        int from;
        int to;
    };
}

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
#define _useFn(name) \
    { name##Fns.data(), int(name##Fns.size()) }

struct RegistryBuilder {
    _makeFns(Load, VoidFn)
    _makeFns(LoadFeats, VoidFn)
    _makeFns(Unload, VoidFn)
    _makeFns(Play, BoolFn)
    _makeFns(Styl, VoidFn)

    Registry build(const char* desc) {
        return Registry{
            _useFn(Load),
            _useFn(LoadFeats),
            _useFn(Unload),
            _useFn(Play),
            _useFn(Styl),
            desc,
        };
    }
};
#undef _makeFns
#undef _useFn

#define REGISTER_PLUG(build, vFrom, vTo) \
    extern "C" Registry _register() { return build; }\
    extern "C" Version _versions() { return  { vFrom, vTo }; }

