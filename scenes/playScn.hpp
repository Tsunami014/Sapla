#pragma once
#include "../items/gridLayout.hpp"
#include "baseScn.hpp"

class PlayScene : public BaseScene {
public:
    PlayScene();
    void onEvent(QEvent* event) override;
    void resize() override;
    void setOverlay(QGraphicsRectItem* newOverlay);
    bool hasOverlay();
    void removeOverlay();
    GLayoutGraphicItem* main;
private:
    QGraphicsRectItem* overlay;
};
