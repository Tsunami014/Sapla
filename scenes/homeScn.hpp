#pragma once
#include "baseScn.hpp"

class HomeScene : public BaseScene {
public:
    HomeScene();
    void onEvent(QEvent* event) override;
    void resize() override;

private:
    QGraphicsTextItem* txt;
};

