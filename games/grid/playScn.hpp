#pragma once
#include "game.hpp"
#include "items/gridLayout.hpp"
#include <QTimer>
#include <QElapsedTimer>
#include <QGraphicsTextItem>

class PlayScene : public BaseScene {
public:
    PlayScene();
    ~PlayScene();
    void onEvent(QEvent* event) override;
    void resize() override;

    void setOverlay(QGraphicsRectItem* newOverlay);
    bool hasOverlay();
    void removeOverlay();

    void pauseTimer();
    void resumeTimer();

    void dialogOpen() override;
    void dialogClose() override;

    GLayoutGraphicItem* main;
    int timeLeft;  // in ms
private:
    void resetTimer();
    int addAnother();

    QGraphicsRectItem* overlay;
    Tree tr;
    ProgressBarItem pb;

    double timeOffset;
    QTimer timer;
    QElapsedTimer elapsed;
};
