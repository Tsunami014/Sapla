#pragma once
#include "baseScn.hpp"
#include "../items/cardTree.hpp"
#include "../items/base/svgBtnItem.hpp"
#include <QGraphicsProxyWidget>
#include <QTreeWidget>
#include <QVBoxLayout>

class BrowseScene : public BaseScene {
public:
    BrowseScene();
    void resize() override;
    void onEvent(QEvent* event) override;

protected:
    SvgBtnItem backBtn;

    QGraphicsProxyWidget TreeProxy;
    CardTree* tree;

    QGraphicsProxyWidget FormProxy;
    QVBoxLayout* form;
};
