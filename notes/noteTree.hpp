#pragma once
#include "note.hpp"
#include <QTreeWidget>

struct TreeData {
    Note* note;
};
Q_DECLARE_METATYPE(TreeData)

Note* getNote(QTreeWidgetItem* item);

QTreeWidget* getNoteTree(QWidget* parent = nullptr);
void resetNoteTree(QTreeWidget* tree);

void updateItem(QTreeWidgetItem* it, Note* note);
void updateAllItems(QTreeWidget* tree);

void filterTree(QTreeWidget* tree, QString filter);

QTreeWidgetItem* addToTree(QTreeWidget* tree, Note* card);

