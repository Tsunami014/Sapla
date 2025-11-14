#include "game.hpp"
#include "playScn.hpp"

bool init() {
    MG->changeScene(new PlayScene());
    return true;
}

void load() {
    Q_INIT_RESOURCE(gridResources);

    colours["fGridIt"] = "#B7C8A9";
    colours["fGridItBg"] = "#5D7B5E";
    colours["fEmpTIt"] = "#30502A";
}
void unload() {
    Q_CLEANUP_RESOURCE(gridResources);
}

REGISTER_PLUG(makeRegistry(1, 1, init, load, unload))

