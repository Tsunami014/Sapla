#pragma once
#include "game.hpp"
#include "items/cardIt.hpp"
#include <QTimer>
#include <QGraphicsTextItem>

class PlayScene : public GameScene {
public:
    PlayScene();
    ~PlayScene();

    bool keyEv(QKeyEvent* event) override;
    void resize() override;
private:
    Tree tr;
    CardGraphicItem* card;
};
