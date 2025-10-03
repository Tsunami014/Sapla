#include "playScn.hpp"

PlayScene::PlayScene() : main(new GLayoutGraphicItem(this)) {}

void PlayScene::resize() {
    main->setRect(rect);
}

