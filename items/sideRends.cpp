#include "sideRends.hpp"

void TextSide::render(QPainter* painter, QRectF rect) {
    painter->drawText(rect, text);
}
void TextSide::renderSelected(QPainter* painter, QRectF rect) {
    painter->drawText(rect, text);
}

