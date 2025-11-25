#include "game.hpp"
#include "playScn.hpp"

bool init() {
    MG->changeScene(new PlayScene());
    return true;
}

void loadCols() {
    colours["fBICardBg1"] = getCol("faded", -50);
    colours["fBICardBg2"] = getCol("faded", 120);
}

auto r = RegistryBuilder()
    .addLoad([](){ Q_INIT_RESOURCE(BIResources); })
    .addUnload([](){ Q_CLEANUP_RESOURCE(BIResources); })
    .addStyl(loadCols)
    .addPlay(init);

REGISTER_PLUG(r, 1, 1)

