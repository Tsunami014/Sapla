#pragma once
#include "baseScn.hpp"
#include "../menu.hpp"
#include "../cards/cardTyps.hpp"
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QMenuBar>

class BrowseScene : public BaseScene {
public:
    BrowseScene();
    void keyPressEvent(QKeyEvent* event) override;

protected:
    QTreeWidget* tree;
    QVBoxLayout* form;

private:
    void addCard(BaseCardTyp* card);
    Menu newCmenu;
};
