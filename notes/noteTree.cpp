#include "noteTree.hpp"
#include "getNotes.hpp"
#include "../wids/listWid.hpp"
#include <QHeaderView>

QTreeWidget* getNoteTree(QWidget* parent) {
    auto* tree = new ListWidget();
    tree->setColumnCount(4);
    tree->setHeaderLabels({"Name", "📚", "📝", "‼️"});
    tree->header()->setSectionResizeMode(0, QHeaderView::Interactive);
    tree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    tree->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    tree->header()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    tree->header()->setMinimumSectionSize(40);
    tree->header()->setDefaultSectionSize(240);

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
    it->setData(3, Qt::DisplayRole, QVariant(note->error.count("\n")));

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

