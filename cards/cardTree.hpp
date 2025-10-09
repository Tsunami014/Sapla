#pragma once
#include "getCards.hpp"
#include <QTreeWidget>

void getCardTree(QTreeWidget* tree);
QTreeWidgetItem* addToTree(QTreeWidget* tree, BaseCardTyp* card);

