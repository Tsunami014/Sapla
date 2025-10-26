#include "game.hpp"
#include "playScn.hpp"

bool init() {
    MG->changeScene(new PlayScene());
    return true;
}

void load() {
    Q_INIT_RESOURCE(BIResources);

    //                        Spring,    Summer,    Automn,    Winter
    colours["fBICardBg1"] = {"#5D7B5E", "#5D7B5E", "#7B6F67", "#5D7B5E"};
    colours["fBICardBg2"] = {"#B7C8A9", "#B7C8A9", "#C8BFA5", "#B7C8A9"};
}
void unload() {
    Q_CLEANUP_RESOURCE(BIResources);
}

REGISTER_GAME(makeRegistry(1, 1, init, load, unload))

