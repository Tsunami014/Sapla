#pragma once
#include "items/treeItem.hpp"
#include "items/deckProgress.hpp"
#include "items/cardIt.hpp"
#include "scenes/gameScn.hpp"
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
    Tree tr;
    DeckProgress dp;
    CardGraphicItem* card;
    QGraphicsTextItem* schdT;
};
