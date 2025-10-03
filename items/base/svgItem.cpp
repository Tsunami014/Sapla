#include "svgItem.hpp"

SvgGraphicItem::SvgGraphicItem(const QString& svgPath, QGraphicsItem* parent) : QGraphicsItem(parent), renderer(svgPath) {}

QRectF SvgGraphicItem::boundingRect() const {
    return rect;
}

void SvgGraphicItem::setRect(const QRectF& newRect) {
    prepareGeometryChange();
    rect = newRect;
}

void SvgGraphicItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    renderer.render(painter, rect);
}
