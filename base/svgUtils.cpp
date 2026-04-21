#include "svgUtils.hpp"
#include "../base/svgRend.hpp"
#include <QTimer>
#include <QCursor>
#include <QPainter>
#include <QGraphicsView>

SvgUtils::SvgUtils(const QString& path) : rend(RenderSvg(path)) { init(); }
SvgUtils::SvgUtils() : rend() { init(); }
void SvgUtils::init() {
    hover = false;
    hlcol = Qt::yellow;
    hlopacity = 0.9;
    hlthick = 3;
}

void SvgUtils::setSvg(const QString& path) {
    rend.load(RenderSvg(path));
}

bool SvgUtils::touching(const QPointF& pos) {
    QRect r = getRect();
    QPointF realpos = {pos.x()-r.x(), pos.y()-r.y()};
    QSize sze = r.size();
    if (realpos.x() < 0 || realpos.y() < 0 || realpos.x() >= sze.width() || realpos.y() >= sze.height())
        return false;
    QImage img(sze, QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::transparent);
    // Paint the svg onto the image
    QPainter p(&img);
    rend.render(&p, QRectF({0, 0}, sze));
    p.end();
    // Find the colour at the pixel of the mouse
    QColor color = img.pixelColor(realpos.toPoint());
    return color.alpha() != 0; // Find if it's transparent
}

void SvgUtils::paintSvg(QPainter* p) {
    paintSvg(p, getRect());
}
void SvgUtils::paintSvg(QPainter* p, const QRect& r) {
    QImage img(r.width(), r.height(), QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::transparent);

    QPainter p1(&img);
    rend.render(&p1, QRectF(QPointF(0, 0), r.size()));
    p1.end();

    p->save();
    if (hover) {
        if (hlopacity != 1.0) p->setOpacity(hlopacity);
        // Create a colorized outline version
        QImage outline = img.copy();
        QPainter p2(&outline);
        p2.setCompositionMode(QPainter::CompositionMode_SourceIn);
        p2.fillRect(outline.rect(), hlcol);
        p2.end();

        p->drawImage(QPoint(r.x()-hlthick, r.y()-hlthick),
            outline.scaled(r.width()+hlthick*2, r.height()+hlthick*2, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }
    p->drawImage(r.topLeft(), img);
    p->restore();
}
