#include "browseScn.hpp"

BrowseScene::BrowseScene() : BaseScene() {
    tl = new TextListItem(this);
    tl->list->addItems({"apple", "banana", "mango", "apricot", "strawberry", "blueberry", "snozzberry", "item 1", "item 2", "item 3"});
}

void BrowseScene::onEvent(QEvent* event) {}
void BrowseScene::resize() {
    tl->setRect({
        rect.width()*0.2, rect.height()*0.05, rect.width()*0.6, rect.height()*0.9
    });
}
