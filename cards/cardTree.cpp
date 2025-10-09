#include "cardTree.hpp"

void getCardTree(QTreeWidget* tree) {
    tree->setColumnCount(1);
    tree->setHeaderLabels({"Name"});

    for (auto* card : cards) {
        addToTree(tree, card);
    }
}

QTreeWidgetItem* addToTree(QTreeWidget* tree, BaseCardTyp* card) {
    QTreeWidgetItem* item = new QTreeWidgetItem(tree);
    item->setText(0, card->getName());

    item->setData(0, Qt::UserRole, QVariant::fromValue<void*>(card));
    //item->setIcon(0, QIcon(":/icons/file.png"));

    return item;
}

