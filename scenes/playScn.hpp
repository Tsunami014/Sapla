#pragma once
#include "../items/gridLayout.hpp"
#include "../items/progress.hpp"
#include "../items/treeItem.hpp"
#include "baseScn.hpp"
#include <QTimer>
#include <QGraphicsTextItem>

struct Stats {
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

    GLayoutGraphicItem* main;
    int timeLeft;  // in ms
private:
    void resetTimer();
    int addAnother();

    Stats s;

    QGraphicsRectItem* overlay;
    Tree tr;
    ProgressBarItem pb;

    double timeOffset;
    QTimer timer;
    QElapsedTimer elapsed;
};
