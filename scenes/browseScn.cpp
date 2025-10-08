#include "browseScn.hpp"
#include "../cards/cardTree.hpp"

BrowseScene::BrowseScene() : BaseScene(), proxy(this) {
    tree = getCardTree();
    proxy.setWidget(tree);
}

void BrowseScene::onEvent(QEvent* event) {}
void BrowseScene::resize() {
    proxy.setPos(0, rect.height()*0.05);
    proxy.resize(rect.width()/2, rect.height()*0.95);
}
