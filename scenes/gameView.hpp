#pragma once
#include "baseScn.hpp"
#include "../menu.hpp"
#include <QTreeWidget>

class GameViewScene : public BaseScene {
public:
    GameViewScene();
public slots:
    void loadF();
private:
    MenuAction m;
    void fillTree();
    QTreeWidget* li;
};

