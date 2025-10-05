#include "myItem.hpp"

MyGraphicsItem::MyGraphicsItem(QGraphicsItem* parent) : QGraphicsItem(parent) {}

QRectF MyGraphicsItem::boundingRect() const {
    return rect;
}

void MyGraphicsItem::setRect(const QRectF& newRect) {
    prepareGeometryChange();
    rect = newRect;
}

