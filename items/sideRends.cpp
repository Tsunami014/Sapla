#include "sideRends.hpp"

#define EQUALS_BEGIN(DerivedType, other) \
    if (typeid(*this) != typeid(other)) return false; \
    const DerivedType& o = static_cast<const DerivedType&>(other);

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
bool TextSide::operator==(const BaseSideRend& other) const {
    EQUALS_BEGIN(TextSide, other)
    return o.text == text;
}

