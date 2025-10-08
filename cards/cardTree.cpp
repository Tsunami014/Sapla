#include "cardTree.hpp"
#include "getCards.hpp"
#include <QHeaderView>

QTreeWidget* getCardTree() {
    QTreeWidget* tree = new QTreeWidget();
    tree->setColumnCount(1);
    tree->setHeaderLabels({"Name"});
    tree->setSortingEnabled(true);
    tree->setSelectionBehavior(QAbstractItemView::SelectRows);
    tree->header()->setSectionResizeMode(QHeaderView::Stretch);

    for (auto* card : cards) {
        QTreeWidgetItem* item = new QTreeWidgetItem(tree);
        item->setText(0, card->getName());

        item->setData(0, Qt::UserRole, QVariant::fromValue<void*>(card));
        //item->setIcon(0, QIcon(":/icons/file.png"));
    }

    return tree;
}
