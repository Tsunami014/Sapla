#pragma once
#include "game.hpp"
#include "items/cardIt.hpp"
#include <QTimer>
#include <QGraphicsTextItem>

class PlayScene : public GameScene {
public:
    PlayScene();
    ~PlayScene();

    void keyPressEvent(QKeyEvent* event) override;
    void layoutIts();
private:
    void resizeEvent(QResizeEvent* event) override;
    Tree tr;
    CardGraphicItem* card;
};
