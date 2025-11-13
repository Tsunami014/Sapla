#include "gameScn.hpp"

GraphicGameScene::GraphicGameScene() : scn(this), view(&scn, this) {
    view.setFocusPolicy(Qt::NoFocus);
    view.setFrameShape(QFrame::NoFrame);
    view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.setBackgroundBrush(Qt::NoBrush);
    view.setStyleSheet("background: transparent;");
    view.viewport()->setAttribute(Qt::WA_TranslucentBackground);
}

void GraphicGameScene::resizeEvent(QResizeEvent* event) {
    view.setGeometry(rect());
    view.setSceneRect(0, 0, width(), height());
    view.resetTransform();
    resize();
}

