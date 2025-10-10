#pragma once
#include "baseScn.hpp"
#include "../items/treeItem.hpp"
#include <QGraphicsTextItem>

struct Stats; // Forward reference

class WinScene : public BaseScene {
public:
    WinScene(Stats stats);
    void onEvent(QEvent* event) override;
    void resize() override;
private:
    QGraphicsTextItem txt;
    Tree tr;
};
