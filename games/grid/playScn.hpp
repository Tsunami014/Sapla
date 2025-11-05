#pragma once
#include "game.hpp"
#include "items/gridLayout.hpp"
#include <QTimer>
#include <QElapsedTimer>
#include <QGraphicsTextItem>

class PlayScene : public GameScene {
public:
    PlayScene();
    ~PlayScene();
    bool keyEv(QKeyEvent* event) override;

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
    void resize() override;
    void resetTimer();
    int addAnother();

    QGraphicsRectItem* overlay;
    Tree tr;
    ProgressBarItem pb;

    double timeOffset;
    QTimer timer;
    QElapsedTimer elapsed;
};
