#pragma once
#include "baseScn.hpp"
#include "../items/base/svgBtnItem.hpp"

class HomeScene : public BaseScene {
public:
    HomeScene();
    void resize() override;

private:
    QGraphicsTextItem* txt;
    SvgBtnItem* playBtn;
};

