#include "svgItem.hpp"
#include <QPainter>

SvgGraphicItem::SvgGraphicItem(const QString& svgPath, QGraphicsItem* parent) : QGraphicsItem(parent) {
    renderer = new QSvgRenderer(svgPath);
}
SvgGraphicItem::~SvgGraphicItem() {
    delete renderer;
}

QRectF SvgGraphicItem::boundingRect() const {
    return rect;
}

void SvgGraphicItem::setRect(const QRectF& newRect) {
    prepareGeometryChange();
    rect = newRect;
}

bool SvgGraphicItem::contains(const QPointF& point) const {
    QImage img(rect.width(), rect.height(), QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::transparent);
    // Paint the svg onto the image
    QPainter p(&img);
    renderer->render(&p, QRectF(QPointF(0, 0), rect.size()));
    p.end();
    // Find the colour at the pixel of the mouse
    QColor color = img.pixelColor(point.x() - rect.x(), point.y() - rect.y());
    return color.alpha() != 0; // Find if it's transparent
}

void SvgGraphicItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    renderer->render(painter, rect);
}
