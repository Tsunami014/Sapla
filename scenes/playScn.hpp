#pragma once
#include "../items/gridLayout.hpp"
#include "../items/progress.hpp"
#include "baseScn.hpp"
#include <QTimer>

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
    void resetTimer(bool add = true);

    double timeOffset;
    QGraphicsRectItem* overlay;
    ProgressBarItem* pb;
    QTimer timer;
    QElapsedTimer elapsed;
};
