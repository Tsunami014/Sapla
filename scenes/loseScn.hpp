#pragma once
#include "baseScn.hpp"
#include <QGraphicsTextItem>

struct Stats; // Forward reference

class LoseScene : public BaseScene {
public:
    LoseScene(Stats stats);
    void onEvent(QEvent* event) override;
    void resize() override;
private:
    QGraphicsTextItem txt;
};
