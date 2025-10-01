#pragma once
#include <QGraphicsView>
#include "items/base/svgItem.hpp"

class MainGame {
public:
    MainGame();
    void resizeEvent(QRectF& newSze);

private:
    SvgGraphicItem* bg;
};
