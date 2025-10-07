#include "listIt.hpp"

TextListItem::TextListItem(QGraphicsItem* parent) : QGraphicsProxyWidget(parent), RectMixin() {
    list = new QListWidget;
    list->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setWidget(list);
}

void TextListItem::setRect(const QRectF& newRect) {
    prepareGeometryChange();
    RectMixin::setRect(newRect);
    setPos(newRect.topLeft());
    resize(newRect.size());
    //list->setGeometry({{0, 0}, boundingRect().size().toSize()});
}

QRectF TextListItem::boundingRect() const {
    return {{0, 0}, RectMixin::rect.size()};//RectMixin::boundingRect();
}

