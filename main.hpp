#pragma once
#include <QMenuBar>
#include <QStackedLayout>
#include <QGraphicsScene>
#include "game.hpp"

#define VERSION 1

extern MainGame* MG;
extern const QString* helpStr;

class BetterScene : public QGraphicsScene {
public:
    BetterScene(QObject *parent = nullptr) : QGraphicsScene(parent) {}

protected:
    bool event(QEvent* event) override {
        MG->curScene->onEvent(event);
        return QGraphicsScene::event(event);
    }
};

extern BetterScene* MScene;

