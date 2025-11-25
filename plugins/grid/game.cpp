#include "game.hpp"
#include "playScn.hpp"

bool init() {
    MG->changeScene(new PlayScene());
    return true;
}

void loadCols() {
    colours["fGridIt"]   = getCol("faded", 120);
    colours["fGridItBg"] = getCol("faded", 60);
    colours["fEmpTIt"]   = getCol("dark",  42);
}

auto r = RegistryBuilder()
    .addLoad([](){ Q_INIT_RESOURCE(gridResources); })
    .addUnload([](){ Q_CLEANUP_RESOURCE(gridResources); })
    .addStyl(loadCols)
    .addPlay(init);

REGISTER_PLUG(r, 1, 1)

