#pragma once
#include "baseScn.hpp"
#include <QGraphicsView>
#include <QGraphicsScene>

class GameScene : public BaseScene {
public:
    GameScene();
    QGraphicsScene scn;
    QGraphicsView view;
protected:
    void resizeEvent(QResizeEvent* event) override;
    virtual void resize() {}
};

