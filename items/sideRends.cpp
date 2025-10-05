#include "sideRends.hpp"

void TextSide::render(QPainter* painter, QRectF rect) {
    painter->setPen(Qt::black);
    rect.setX(rect.x() + rect.width()*0.1);
    rect.setWidth(rect.width()*0.8);
    rect.setY(rect.y() + rect.height()*0.1);
    rect.setHeight(rect.height()*0.8);
    painter->drawText(rect, Qt::AlignCenter | Qt::TextWordWrap, text);
}
void TextSide::renderSelected(QPainter* painter, QRectF rect) {
    TextSide::render(painter, rect);
}

