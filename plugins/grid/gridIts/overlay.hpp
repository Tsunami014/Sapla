#pragma once
#include <QGraphicsRectItem>
#include <QBrush>
#include <QPen>

class Overlay : public QGraphicsRectItem {
public:
    Overlay(QGraphicsItem *parent = nullptr) : QGraphicsRectItem(parent) {
        setBrush(QBrush(QColor(125, 125, 125, 125)));
        setPen(Qt::NoPen);
    }
};
