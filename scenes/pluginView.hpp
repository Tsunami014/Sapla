#pragma once
#include "baseScn.hpp"
#include "../menu.hpp"
#include <QTreeWidget>
#include <QTextEdit>

class PlugViewScene : public BaseScene {
public:
    PlugViewScene();
public slots:
    void loadF();
private:
    MenuAction m;
    void fillTree();
    void reset();
    QTreeWidget* li;
    QTextEdit* txt;
};

