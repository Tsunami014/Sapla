#include "playScn.hpp"
#include "../cards/getCards.hpp"

PlayScene::PlayScene() : main(new GLayoutGraphicItem(this)) {
    overlay = NULL;
    main->addItem(cards[0]->getItem());
}

void PlayScene::resize() {
    main->setRect(rect);
    if (overlay != NULL) {
        overlay->setRect(rect);
    }
}

void PlayScene::setOverlay(QGraphicsRectItem* ovl) {
    overlay = ovl;
    overlay->setRect(rect);
}
bool PlayScene::hasOverlay() {
    return overlay != NULL;
}
void PlayScene::removeOverlay() {
    delete overlay;
    overlay = NULL;
}

