#pragma once
extern "C" struct Registry {
    void (*loadFn)();
    bool (*runFn)();
    void (*unloadFn)();
    int vFrom;
    int vTo;
};
