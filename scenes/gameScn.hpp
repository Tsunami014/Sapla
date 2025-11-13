#pragma once
#include "baseScn.hpp"
#include <QGraphicsView>
#include <QGraphicsScene>

class GameScene : public BaseScene {
public:
    GameScene() {}
    virtual void resume() {}
};
class GraphicGameScene : public BaseScene {
public:
    GraphicGameScene();
    QGraphicsScene scn;
    QGraphicsView view;
    virtual void resume() {}
protected:
    void resizeEvent(QResizeEvent* event) override;
    virtual void resize() {}
};

