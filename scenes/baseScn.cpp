#include "baseScn.hpp"
#include "../core.hpp"

BaseScene::BaseScene() : QWidget(MG) {
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_TranslucentBackground);
}

void BaseScene::keyPressEvent(QKeyEvent* event) {
    MG->handleEv(event);
}

