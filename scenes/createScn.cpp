#include "createScn.hpp"

CreateScene::CreateScene() : BaseScene() {
    tl = new TextListItem(this);
    tl->list->addItems({"apple", "banana", "mango", "apricot", "strawberry", "blueberry", "snozzberry", "item 1", "item 2", "item 3"});
}

void CreateScene::onEvent(QEvent* event) {}
void CreateScene::resize() {
    tl->setRect({
        rect.width()*0.2, rect.height()*0.05, rect.width()*0.6, rect.height()*0.9
    });
}
