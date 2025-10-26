#pragma once
#include "game.hpp"
#include "items/cardIt.hpp"
#include <QTimer>
#include <QGraphicsTextItem>

class PlayScene : public BaseScene {
public:
    PlayScene();
    ~PlayScene();
    void onEvent(QEvent* event) override;
    void resize() override;
private:
    Tree tr;
    CardGraphicItem* card;
};
