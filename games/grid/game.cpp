#include "game.hpp"
#include "playScn.hpp"

bool init() {
    MG->changeScene(new PlayScene());
    return true;
}

void load() {
    Q_INIT_RESOURCE(gridResources);
    //                       Spring,    Summer,    Automn,    Winter
    colours["fGridIt"] =   {"#B7C8A9", "#B7C8A9", "#C8BFA5", "#B7C8A9"};
    colours["fGridItBg"] = {"#5D7B5E", "#5D7B5E", "#7B6F67", "#5D7B5E"};
    colours["fEmpTIt"] =   {"#30502A", "#30502A", "#754C2B", "#30502A"};
}
void unload() {
    Q_CLEANUP_RESOURCE(gridResources);
}

REGISTER_GAME(makeRegistry(1, 1, init, load, unload))

