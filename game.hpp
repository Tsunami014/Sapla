#pragma once
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include "scenes/baseScn.hpp"
#include "items/base/svgItem.hpp"
#include "items/base/svgBtnItem.hpp"

class MainGame {
public:
    MainGame();
    void initScene();
    void changeBG(QString bgName);
    void resizeEvent(const QRectF& newSze);
    void changeScene(BaseScene* newScene);
    void nextFC();
    BaseScene* curScene;
    std::vector<TxtBtnItem*> logs;

    void setBottomTxt(const QString& newTxt);

private:
    SvgGraphicItem* bg;
    QGraphicsTextItem* bottomTxt;
};
