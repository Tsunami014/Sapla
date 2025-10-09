#pragma once
#include "baseScn.hpp"
#include "../items/cardTree.hpp"
#include <QGraphicsProxyWidget>
#include <QTreeWidget>
#include <QVBoxLayout>

class BrowseScene : public BaseScene {
public:
    BrowseScene();
    void resize() override;

protected:
    QGraphicsProxyWidget TreeProxy;
    CardTree* tree;

    QGraphicsProxyWidget FormProxy;
    QVBoxLayout* form;
};
