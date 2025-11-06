#pragma once
#include "baseScn.hpp"
#include "../menu.hpp"
#include <QTreeWidget>
#include <QTextEdit>

class GameViewScene : public BaseScene {
public:
    GameViewScene();
public slots:
    void loadF();
private:
    MenuAction m;
    void fillTree();
    void reset();
    QTreeWidget* li;
    QTextEdit* txt;
};

