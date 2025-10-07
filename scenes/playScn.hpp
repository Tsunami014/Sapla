#pragma once
#include "../items/gridLayout.hpp"
#include "../items/progress.hpp"
#include "../items/treeItem.hpp"
#include "baseScn.hpp"
#include <QTimer>
#include <QGraphicsTextItem>

struct Stats {
    int coins;
    int successes;
    int faliures;
};

class PlayScene : public BaseScene {
public:
    PlayScene();
    void onEvent(QEvent* event) override;
    void resize() override;
    void setOverlay(QGraphicsRectItem* newOverlay);
    bool hasOverlay();
    void removeOverlay();

    void pauseTimer();
    void resumeTimer();

    void increaseCoins(int amnt);

    GLayoutGraphicItem* main;
    int timeLeft;  // in ms
private:
    void resetTimer(bool add = true);

    Stats s;
    int coins;
    int skipCoins;

    QGraphicsRectItem* overlay;
    QGraphicsTextItem coinsTxt;
    Tree tr;
    ProgressBarItem pb;

    double timeOffset;
    QTimer timer;
    QElapsedTimer elapsed;
};
