#pragma once
#include <QGraphicsView>
#include "items/base/svgItem.hpp"
#include "scenes/baseScn.hpp"

class MainGame {
public:
    MainGame(BaseScene* scene);
    void resizeEvent(QRectF& newSze);
    void changeScene(BaseScene* newScene);

private:
    SvgGraphicItem* bg;
    BaseScene* curScene;
};
