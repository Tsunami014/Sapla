#include "noteTree.hpp"
#include "getNotes.hpp"
#include "filter.hpp"
#include "../wids/listWid.hpp"
#include <QHeaderView>

Note* getNote(QTreeWidgetItem* item) {
    return static_cast<TreeData*>(item->data(0, Qt::UserRole).value<void*>())->note;
}

QTreeWidget* getNoteTree(QWidget* parent) {
    auto* tree = new ListWidget();
    int cols = 6;
    tree->setColumnCount(cols);
    tree->setHeaderLabels({"Name", "📚", "📝", "%", "🏷️", "‼️"});
    auto header = tree->header();
    header->setStretchLastSection(false);
    for (int i = 1; i < cols; i++) {
        header->setSectionResizeMode(i, QHeaderView::ResizeToContents);
    }
    header->setSectionResizeMode(0, QHeaderView::Interactive);
    header->setSectionResizeMode(cols-2, QHeaderView::Interactive);
    header->setMinimumSectionSize(40);
    header->setDefaultSectionSize(180);

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
    if (note->error.isEmpty()) {
        it->setText(0, note->title());
    } else {
        it->setText(0, "‼️"+note->title());
    }
    it->setData(1, Qt::DisplayRole, QVariant(note->getNumCards()));
    it->setData(2, Qt::DisplayRole, QVariant(note->getNumTemplates()));
    it->setData(3, Qt::DisplayRole, QVariant(std::round(note->percentage()*100)));
    it->setText(4, note->tags.join(", "));
    it->setData(5, Qt::DisplayRole, QVariant(note->error.count("\n")));

    //item->setIcon(0, QIcon(":/icons/file.png"));
}

void updateAllItems(QTreeWidget* tree) {
    int count = tree->topLevelItemCount();
    for (int i = 0; i < count; ++i) {
        QTreeWidgetItem* item = tree->topLevelItem(i);
        updateItem(item, getNote(item));
    }
}

QTreeWidgetItem* addToTree(QTreeWidget* tree, Note* note) {
    QTreeWidgetItem* item = new QTreeWidgetItem(tree);
    updateItem(item, note);

    item->setData(0, Qt::UserRole, QVariant::fromValue<void*>(
        new TreeData{note}
    ));
    return item;
}

void filterTree(QTreeWidget* tree, QString filter) {
    int count = tree->topLevelItemCount();
    for (int i = 0; i < count; ++i) {
        QTreeWidgetItem* item = tree->topLevelItem(i);
        item->setHidden(!matchesFilter(getNote(item), filter));
    }
}
void filterItem(QTreeWidgetItem* it, QString filter) {
    it->setHidden(!matchesFilter(getNote(it), filter));
}
