#include "formElm.hpp"

void LabelFormElm::render(QPainter* painter, QRectF& rect) {
    painter->drawText(rect, text);
}
