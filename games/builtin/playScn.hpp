#pragma once
#include "game.hpp"
#include "items/cardIt.hpp"
#include <QTimer>
#include <QGraphicsTextItem>

class PlayScene : public GraphicGameScene {
public:
    PlayScene();
    ~PlayScene();

    bool keyEv(QKeyEvent* event) override;
    void resume() override;
    void resize() override;
private:
    bool done;
    Tree& tr;
    CardGraphicItem* card;
};
