#pragma once
#include "baseScn.hpp"
#include <QGraphicsProxyWidget>
#include <QTreeWidget>

class BrowseScene : public BaseScene {
public:
    BrowseScene();
    void onEvent(QEvent* event) override;
    void resize() override;

protected:
    QGraphicsProxyWidget proxy;
    QTreeWidget* tree;
};
