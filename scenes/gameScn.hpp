#pragma once
#include "baseScn.hpp"
#include <QGraphicsView>
#include <QGraphicsScene>

class GameScene : public BaseScene {
public:
    GameScene() {}
    virtual void resume() {}
};
class GraphicGameScene : public GameScene {
public:
    GraphicGameScene();
    QGraphicsScene scn;
    QGraphicsView view;
protected:
    void resizeEvent(QResizeEvent* event) override;
    virtual void resize() {}
};

