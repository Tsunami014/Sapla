#include "cardTree.hpp"
#include "cardList.hpp"
#include "../items/listWid.hpp"
#include <QHeaderView>

QTreeWidget* getCardTree() {
    auto* tree = new ListWidget();
    tree->setColumnCount(2);
    tree->setHeaderLabels({"Name", "Type"});
    tree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    tree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

    for (auto* card : getCards()) {
        addToTree(tree, card);
    }
    return tree;
}

QTreeWidgetItem* addToTree(QTreeWidget* tree, BaseCardTyp* card) {
    QTreeWidgetItem* item = new QTreeWidgetItem(tree);
    item->setText(0, card->getName());
    item->setText(1, card->typeName());

    item->setData(0, Qt::UserRole, QVariant::fromValue<void*>(
        new TreeData{card, true}
    ));
    //item->setIcon(0, QIcon(":/icons/file.png"));

    return item;
}

