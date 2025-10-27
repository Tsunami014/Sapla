#include "baseScn.hpp"
#include "../core.hpp"

void BaseScene::onEvent(QEvent* event) {
    MG->handleEv(event);
}
void BaseScene::setRect(const QRectF& newRect) {
    prepareGeometryChange();
    rect = newRect;
    resize();
}

