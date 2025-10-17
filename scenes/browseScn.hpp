#pragma once
#include "baseScn.hpp"
#include "../items/base/svgBtnItem.hpp"
#include "../cards/cardTyps.hpp"
#include <QGraphicsProxyWidget>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QMenuBar>

class BrowseScene : public BaseScene {
public:
    BrowseScene();
    void resize() override;
    void onEvent(QEvent* event) override;

protected:
    SvgBtnItem backBtn;

    QGraphicsProxyWidget TreeProxy;
    QTreeWidget* tree;

    QGraphicsProxyWidget FormProxy;
    QVBoxLayout* form;
    QMenuBar* fmenu;

private:
    void addCard(BaseCardTyp* card);
};
