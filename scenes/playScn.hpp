#pragma once
#include "items/gridLayout.hpp"
#include "baseScn.hpp"

class PlayScene : public BaseScene {
public:
    PlayScene();
    void resize() override;
private:
    GLayoutGraphicItem* main;
};
