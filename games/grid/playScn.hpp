#pragma once
#include "game.hpp"
#include "items/gridLayout.hpp"
#include <QGraphicsTextItem>

class PlayScene : public GraphicGameScene {
public:
    PlayScene();
    ~PlayScene();

    bool keyEv(QKeyEvent* event) override;
    void resume() override;
    void resize() override;

    void setOverlay(QGraphicsRectItem* newOverlay);
    bool hasOverlay();
    void removeOverlay();

    GLayoutGraphicItem* main;
private:
    QGraphicsRectItem* overlay;
    Tree& tr;
};
