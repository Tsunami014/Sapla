#pragma once
#include "baseScn.hpp"

class GameViewScene : public BaseScene {
public:
    GameViewScene();
    void resize() override;
    void onEvent(QEvent* event) override;
};

