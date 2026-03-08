#pragma once
#include "items/treeItem.hpp"
#include "items/deckProgress.hpp"
#include "items/scheduleInfTxt.hpp"
#include "gridIts/gridLayout.hpp"
#include "scenes/gameScn.hpp"
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
    ScheduleInfoTxt* schdT;
    Tree tr;
    DeckProgress dp;
};
