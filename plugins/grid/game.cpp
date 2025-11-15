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
    colours["fGridIt"]   = getCol("faded", {78, 44, 62});
    colours["fGridItBg"] = getCol("faded", {-12, -33, -12});
    colours["fEmpTIt"]   = getCol("dark",  {15, 10, 15});
}
void unload() {
    Q_CLEANUP_RESOURCE(gridResources);
}

REGISTER_PLUG(makeRegistry(1, 1, init, load, unload, loadCols))

