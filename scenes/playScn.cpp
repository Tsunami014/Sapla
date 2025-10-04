#include "playScn.hpp"
#include "../cards/getCards.hpp"
#include "../main.hpp"
#include <QEvent>
#include <QKeyEvent>

PlayScene::PlayScene() : main(new GLayoutGraphicItem(this)) {
    overlay = NULL;
    for (auto& c : cards) {
        main->addItem(c->getItem());
    }
}

void PlayScene::onEvent(QEvent* event) {
    if (event->type() == QEvent::KeyPress) {
        auto* keyEvent = (QKeyEvent*)event;
        int key = keyEvent->key();

        if (key == Qt::Key_Space && overlay != NULL) {
            for (auto& it : main->grid) {
                if (it.item->side == 255) {
                    it.item->finish();
                    break;
                }
            }
        }
    }
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
    MScene->removeItem(overlay);
    overlay = NULL;
}

