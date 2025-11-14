#pragma once
#include "baseScn.hpp"
#include <QGraphicsTextItem>

class WinScene : public BaseScene {
public:
    WinScene();
    bool keyEv(QKeyEvent* event) override;
};
