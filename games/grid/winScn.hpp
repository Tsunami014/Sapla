#pragma once
#include "game.hpp"
#include <QGraphicsTextItem>

class WinScene : public BaseScene {
public:
    WinScene();
    void onEvent(QEvent* event) override;
    void resize() override;
private:
    QGraphicsTextItem txt;
    Tree tr;
};
