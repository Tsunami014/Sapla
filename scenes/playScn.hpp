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

    GLayoutGraphicItem* main;
    int timeLeft;  // in ms
private:
    void resetTimer();

    QGraphicsRectItem* overlay;
    ProgressBarItem* pb;
    QTimer timer;
    QElapsedTimer elapsed;
};
