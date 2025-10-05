#pragma once
#include "baseScn.hpp"
#include <QGraphicsTextItem>

class LoseScene : public BaseScene {
public:
    LoseScene();
    void onEvent(QEvent* event) override;
    void resize() override;
private:
    QGraphicsTextItem txt;
};
