#include "cardTree.hpp"
#include "getCards.hpp"

void getCardTree(QTreeWidget* tree) {
    tree->setColumnCount(1);
    tree->setHeaderLabels({"Name"});

    for (auto* card : cards) {
        QTreeWidgetItem* item = new QTreeWidgetItem(tree);
        item->setText(0, card->getName());

        item->setData(0, Qt::UserRole, QVariant::fromValue<void*>(card));
        //item->setIcon(0, QIcon(":/icons/file.png"));
    }
}
