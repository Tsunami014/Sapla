#include "svgUtils.hpp"
#include "../base/svgRend.hpp"
#include <QTimer>
#include <QCursor>
#include <QPainter>

SvgUtils::SvgUtils(const QString& path) : rend(RenderSvg(path)) {
    hover = false;
    QTimer::singleShot(0, [this](){
        hover = touching(QCursor::pos());
    });
}
SvgUtils::SvgUtils() : rend() {
    hover = false;
}

void SvgUtils::setSvg(const QString& path) {
    rend.load(RenderSvg(path));
}

bool SvgUtils::touching(const QPointF& pos) {
    QRect r = getRect();
    QImage img(r.size(), QImage::Format_ARGB32_Premultiplied);
    if (pos.x() < 0 || pos.y() < 0 || pos.x() >= img.width() || pos.y() >= img.height())
        return false;
    img.fill(Qt::transparent);
    // Paint the svg onto the image
    QPainter p(&img);
    rend.render(&p, r);
    p.end();
    // Find the colour at the pixel of the mouse
    QColor color = img.pixelColor({int(pos.x()), int(pos.y())});
    return color.alpha() != 0; // Find if it's transparent
}

void SvgUtils::paintSvg(QPainter* painter) {
    paintSvg(painter, getRect());
}
void SvgUtils::paintSvg(QPainter* painter, const QRect& r) {
    QImage img(r.width(), r.height(), QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::transparent);

    QPainter p1(&img);
    rend.render(&p1, QRectF(QPointF(0, 0), r.size()));
    p1.end();

    if (hover) {
        painter->save();
        painter->setOpacity(0.8);
        // Create a colorized outline version
        QImage outline = img.copy();
        QPainter p2(&outline);
        p2.setCompositionMode(QPainter::CompositionMode_SourceIn);
        p2.fillRect(outline.rect(), Qt::yellow);
        p2.end();

        painter->drawImage(QPoint(-2, -2), outline.scaled(r.width()+4, r.height()+4, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }
    painter->drawImage(QPoint(0, 0), img);
    if (hover) {
        painter->restore();
    }
}
