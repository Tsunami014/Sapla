#pragma once
#include "baseScn.hpp"
#include "../menu.hpp"

class GameViewScene : public BaseScene {
public:
    GameViewScene();
public slots:
    void loadF();
private:
    MenuAction m;
};

