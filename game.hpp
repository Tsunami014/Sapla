#pragma once
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include "scenes/baseScn.hpp"
#include "items/base/svgItem.hpp"

class MainGame {
public:
    MainGame();
    void initScene();
    void resizeEvent(const QRectF& newSze);
    void changeScene(BaseScene* newScene);
    BaseScene* curScene;

    void setBottomTxt(const QString& newTxt);

private:
    SvgGraphicItem* bg;
    QGraphicsTextItem* bottomTxt;
};
