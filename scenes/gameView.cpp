#include "../core.hpp"
#include "gameView.hpp"
#include "homeScn.hpp"

GameViewScene::GameViewScene()
    : BaseScene() {
        MG->changeBG("dirt");
    }

void GameViewScene::onEvent(QEvent* event) {
    MG->handleEv(event);
}
void GameViewScene::resize() {}

