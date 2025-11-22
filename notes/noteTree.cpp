#include "noteTree.hpp"
#include "getNotes.hpp"
#include "../wids/listWid.hpp"
#include <QHeaderView>

QTreeWidget* getNoteTree(QWidget* parent) {
    auto* tree = new ListWidget();
    tree->setColumnCount(3);
    tree->setHeaderLabels({"Name", "Cards", "Templates"});
    tree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    tree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

    resetNoteTree(tree);
    return tree;
}
void resetNoteTree(QTreeWidget* tree) {
    tree->clear();
    for (auto* note : notesL) {
        addToTree(tree, note);
    }
    tree->sortItems(
        tree->header()->sortIndicatorSection(),
        tree->header()->sortIndicatorOrder()
    );
}

void updateItem(QTreeWidgetItem* it, Note* note) {
    it->setText(0, note->title());
    it->setData(1, Qt::DisplayRole, QVariant(note->getNumCards()));
    it->setData(2, Qt::DisplayRole, QVariant(note->getNumTemplates()));

    //item->setIcon(0, QIcon(":/icons/file.png"));
}

QTreeWidgetItem* addToTree(QTreeWidget* tree, Note* note) {
    QTreeWidgetItem* item = new QTreeWidgetItem(tree);
    updateItem(item, note);

    item->setData(0, Qt::UserRole, QVariant::fromValue<void*>(
        new TreeData{note}
    ));
    return item;
}

void updateNoteOnTree(QTreeWidgetItem* it, QString nc) {
    Note* n = static_cast<TreeData*>(it->data(0, Qt::UserRole).value<void*>())->note;
    n->setContents(nc);
    n->updateCards();

    updateItem(it, n);
}

