#include "svgItem.hpp"
#include "svgRend.hpp"
#include <QPainter>

SvgGraphicItem::SvgGraphicItem(const QString& svgPath, QGraphicsItem* parent) : RectItem(parent) {
    renderer = new QSvgRenderer(RenderSvg(svgPath));
}
SvgGraphicItem::SvgGraphicItem(QGraphicsItem* parent) : RectItem(parent) {
    renderer = new QSvgRenderer();
}
SvgGraphicItem::~SvgGraphicItem() {
    delete renderer;
}

bool SvgGraphicItem::contains(const QPointF& point) const {
    QPoint newP(point.x() - rect.x(), point.y() - rect.y());
    if (newP.x() < 0 || newP.y() < 0) {
        return false;
    }
    if (newP.x() >= (int)rect.width() || newP.y() >= (int)rect.height()) {
        return false;
    }
    QImage img(rect.width(), rect.height(), QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::transparent);
    // Paint the svg onto the image
    QPainter p(&img);
    renderer->render(&p, QRectF(QPointF(0, 0), rect.size()));
    p.end();
    // Find the colour at the pixel of the mouse
    QColor colour = img.pixelColor(newP);
    return colour.alpha() != 0; // Find if it's transparent
}

void SvgGraphicItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    renderer->render(painter, rect);
}
