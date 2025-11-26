#pragma once
#include "note.hpp"
#include <QTreeWidget>

struct TreeData {
    Note* note;
};
Q_DECLARE_METATYPE(TreeData)

QTreeWidget* getNoteTree(QWidget* parent = nullptr);
void resetNoteTree(QTreeWidget* tree);
void updateItem(QTreeWidgetItem* it, Note* note);
QTreeWidgetItem* addToTree(QTreeWidget* tree, Note* card);

