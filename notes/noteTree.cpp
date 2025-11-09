#include "noteTree.hpp"
#include "getNotes.hpp"
#include "../wids/listWid.hpp"
#include <QHeaderView>

QTreeWidget* getNoteTree(QWidget* parent) {
    auto* tree = new ListWidget();
    tree->setColumnCount(1);
    tree->setHeaderLabels({"Name"});
    tree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    tree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

    for (auto& note : notesL) {
        addToTree(tree, &note);
    }
    return tree;
}

QTreeWidgetItem* addToTree(QTreeWidget* tree, Note* note) {
    QTreeWidgetItem* item = new QTreeWidgetItem(tree);
    item->setText(0, note->title());

    item->setData(0, Qt::UserRole, QVariant::fromValue<void*>(
        new TreeData{note}
    ));
    //item->setIcon(0, QIcon(":/icons/file.png"));

    return item;
}

