#pragma once
#include "baseScn.hpp"
#include "../items/listWid.hpp"
#include <QGraphicsProxyWidget>

class GameViewScene : public BaseScene {
public:
    GameViewScene();
    void resize() override;
    void onEvent(QEvent* event) override;
private:
    ListWidget li;
    QGraphicsProxyWidget prox;
};

