#pragma once
extern "C" struct Registry {
    void (*loadFn)();
    void (*loadColsFn)();
    bool (*runFn)();
    void (*unloadFn)();
    int vFrom;
    int vTo;
};
