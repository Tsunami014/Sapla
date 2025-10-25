#pragma once
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include "scenes/baseScn.hpp"
#include "items/base/svgItem.hpp"
#include "items/base/svgBtnItem.hpp"

struct Stats {  // TODO: Save/load
    uint32_t goods;
    uint32_t bads;
};

class MainGame {
public:
    MainGame();
    void initScene();
    void changeBG(QString bgName);
    void resizeEvent(const QRectF& newSze);
    void updateLogs();
    void changeScene(BaseScene* newScene);
    void nextFC();
    BaseScene* curScene;
    std::vector<TxtBtnItem*> logs;
    Stats s;

private:
    SvgGraphicItem* bg;
};
