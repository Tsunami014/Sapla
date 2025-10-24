#include "game.hpp"
#include "playScn.hpp"

const Link* ln = nullptr;
bool init(const Link* link) {
    ln = link;
    ln->MG->changeScene(new PlayScene());
    return true;
}

REGISTER_GAME(init, 1, 1)
