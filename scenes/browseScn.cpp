#include "browseScn.hpp"
#include "../cards/cardTree.hpp"
#include "../cards/cardTyps.hpp"

BrowseScene::BrowseScene() : BaseScene(), proxy(this) {
    tree = new CardTree();
    getCardTree(tree);

    QWidget::connect(tree, &QTreeWidget::itemClicked, [&](QTreeWidgetItem* item, int column){
        BaseCardTyp* data = static_cast<BaseCardTyp*>(item->data(0, Qt::UserRole).value<void*>());
        qDebug() << "Clicked item:" << data->getName();
    });
    proxy.setWidget(tree);
}

void BrowseScene::onEvent(QEvent* event) {}
void BrowseScene::resize() {
    proxy.setPos(0, rect.height()*0.05);
    proxy.resize(rect.width()/2, rect.height()*0.95);
}
