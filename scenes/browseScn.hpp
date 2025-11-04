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
    bool keyEv(QKeyEvent* event) override;

protected:
    QTreeWidget* tree;
    QVBoxLayout* form;

private:
    void addCard(BaseCardTyp* card);
    Menu newCmenu;
};
