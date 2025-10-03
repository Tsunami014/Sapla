#include "playScn.hpp"
#include "../cards/getCards.hpp"

PlayScene::PlayScene() : main(new GLayoutGraphicItem(this)) {
    main->addItem(cards[0]->getItem());
}

void PlayScene::resize() {
    main->setRect(rect);
}

