#pragma once
#include <QGraphicsView>
#include "scenes/baseScn.hpp"
#include "items/base/svgItem.hpp"

class MainGame {
public:
    MainGame(BaseScene* scene);
    void resizeEvent(QRectF& newSze);
    void changeScene(BaseScene* newScene);

private:
    SvgGraphicItem* bg;
    BaseScene* curScene;
};
