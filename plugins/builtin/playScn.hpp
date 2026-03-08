#pragma once
#include "menu.hpp"
#include "items/treeItem.hpp"
#include "items/deckProgress.hpp"
#include "items/cardIt.hpp"
#include "items/scheduleInfTxt.hpp"
#include "scenes/gameScn.hpp"
#include <QTimer>

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
    ScheduleInfoTxt* schdT;
    GameMenues ms;
};
