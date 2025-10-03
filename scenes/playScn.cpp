#include "playScn.hpp"

PlayScene::PlayScene() : main(new LayoutGraphicItem(this)) {}

void PlayScene::resize() {
    main->setRect(rect);
}

