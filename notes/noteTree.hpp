#pragma once
#include "note.hpp"
#include <QTreeWidget>

struct TreeData {
    Note* note;
};
Q_DECLARE_METATYPE(TreeData)

QTreeWidget* getNoteTree(QWidget* parent = nullptr);
void resetNoteTree(QTreeWidget* tree);
QTreeWidgetItem* addToTree(QTreeWidget* tree, Note* card);
void updateNoteOnTree(QTreeWidgetItem* it, QString newContents);

