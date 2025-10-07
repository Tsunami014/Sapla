#include "rectIt.hpp"

QRectF RectMixin::boundingRect() const {
    return rect;
}

void RectMixin::setRect(const QRectF& newRect) {
    rect = newRect;
}

RectItem::RectItem(QGraphicsItem* parent) : QGraphicsItem(parent), RectMixin() {}

void RectItem::setRect(const QRectF& newRect) {
    prepareGeometryChange();
    RectMixin::setRect(newRect);
}

QRectF RectItem::boundingRect() const {
     return RectMixin::boundingRect();
}

