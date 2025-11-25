#pragma once
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
