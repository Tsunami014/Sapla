#include "browseScn.hpp"
#include "../cards/cardTree.hpp"
#include "../cards/cardTyps.hpp"

BrowseScene::BrowseScene() : BaseScene(), proxy(this) {
    tree = new CardTree();
    getCardTree(tree);

    QWidget::connect(tree, &QTreeWidget::itemSelectionChanged, [&](){
        QList<QTreeWidgetItem*> selected = tree->selectedItems();
        if (selected.isEmpty())
            return;

        QTreeWidgetItem* item = selected.first();
        BaseCardTyp* data = static_cast<BaseCardTyp*>(item->data(0, Qt::UserRole).value<void*>());
        qDebug() << "Selected item:" << data->getName();
    });
    proxy.setWidget(tree);
}

void BrowseScene::onEvent(QEvent* event) {}
void BrowseScene::resize() {
    proxy.setPos(0, rect.height()*0.05);
    proxy.resize(rect.width()/2, rect.height()*0.95);
}
