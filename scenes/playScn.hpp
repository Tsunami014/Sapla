#pragma once
#include "items/layout.hpp"
#include "baseScn.hpp"

class PlayScene : public BaseScene {
public:
    PlayScene();
    void resize() override;
private:
    LayoutGraphicItem* main;
};
