#pragma once
#include "getCards.hpp"
#include <QTreeWidget>

struct TreeData {
    BaseCardTyp* card;
    bool isBase;
};
Q_DECLARE_METATYPE(TreeData)

QTreeWidget* getCardTree(QWidget* parent = nullptr);
QTreeWidgetItem* addToTree(QTreeWidget* tree, BaseCardTyp* card);

