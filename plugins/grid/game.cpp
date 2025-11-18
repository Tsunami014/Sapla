#include "game.hpp"
#include "playScn.hpp"

bool init() {
    MG->changeScene(new PlayScene());
    return true;
}

void load() {
    Q_INIT_RESOURCE(gridResources);
}
void loadCols() {
    colours["fGridIt"]   = getCol("faded", 120);
    colours["fGridItBg"] = getCol("faded", 60);
    colours["fEmpTIt"]   = getCol("dark",  42);
}
void unload() {
    Q_CLEANUP_RESOURCE(gridResources);
}

REGISTER_PLUG(makeRegistry(1, 1, init, load, unload, loadCols))

