#pragma once
#include "note.hpp"
#include <QTreeWidget>

struct TreeData {
    Note* note;
};
Q_DECLARE_METATYPE(TreeData)

QTreeWidget* getNoteTree(QWidget* parent = nullptr);
QTreeWidgetItem* addToTree(QTreeWidget* tree, Note* card);

