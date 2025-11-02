#pragma once
#include "baseScn.hpp"
#include <QGraphicsView>
#include <QGraphicsScene>

class GameScene : public BaseScene {
public:
    GameScene();
protected:
    void resizeEvent(QResizeEvent* event) override;
    QGraphicsScene scn;
    QGraphicsView view;
};

