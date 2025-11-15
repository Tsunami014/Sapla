#include "game.hpp"
#include "playScn.hpp"

bool init() {
    MG->changeScene(new PlayScene());
    return true;
}

void load() {
    Q_INIT_RESOURCE(BIResources);
}
void loadCols() {
    colours["fBICardBg1"] = getCol("faded", {-10, -33, -13});
    colours["fBICardBg2"] = getCol("faded", {80, 44, 62});
}
void unload() {
    Q_CLEANUP_RESOURCE(BIResources);
}

REGISTER_PLUG(makeRegistry(1, 1, init, load, unload, loadCols))

