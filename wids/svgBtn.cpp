#include "svgBtn.hpp"
#include "../base/svgRend.hpp"
#include <QPainter>
#include <QMouseEvent>

SvgBtn::SvgBtn(QWidget* parnt)
    : QLabel(parnt), rend(new QSvgRenderer(this)) {
        init();
    }
SvgBtn::SvgBtn(const QString& pth, QWidget* parnt)
    : QLabel(parnt), rend(new QSvgRenderer(RenderSvg(pth), this)) {
        init();
    }
void SvgBtn::init() {
    hover = false;
    setWordWrap(true);
    setAlignment(Qt::AlignCenter);
    setMouseTracking(true);
    setContentsMargins(40, 5, 40, 5);
}

void SvgBtn::leaveEvent(QEvent* event) {
    hover = false;
    unsetCursor();
    update();
}
void SvgBtn::mouseMoveEvent(QMouseEvent* event) {
    if (touching(event->pos())) {
        if (!hover) {
            hover = true;
            setCursor(Qt::PointingHandCursor);
            update();
        }
    } else if (hover) {
        hover = false;
        unsetCursor();
        update();
    }
}
bool SvgBtn::touching(const QPointF& pos) {
    QImage img(size(), QImage::Format_ARGB32_Premultiplied);
    if (pos.x() < 0 || pos.y() < 0 || pos.x() >= img.width() || pos.y() >= img.height())
        return false;
    img.fill(Qt::transparent);
    // Paint the svg onto the image
    QPainter p(&img);
    rend->render(&p, rect());
    p.end();
    // Find the colour at the pixel of the mouse
    QColor color = img.pixelColor({int(pos.x()), int(pos.y())});
    return color.alpha() != 0; // Find if it's transparent
}
void SvgBtn::mousePressEvent(QMouseEvent* event) {
    if (touching(mapFromGlobal(QCursor::pos()))) {
        emit clicked();
    }
}

void SvgBtn::setSvg(const QString& path) {
    rend->load(RenderSvg(path));
}

void SvgBtn::paintEvent(QPaintEvent* event) {
    constexpr int rPad = 5;
    QRectF r = rect().adjusted(rPad, rPad, -rPad, -rPad);
    QImage img(r.width(), r.height(), QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::transparent);
    QPainter p1(&img);
    rend->render(&p1, QRectF(QPointF(0, 0), r.size()));
    p1.end();

    QPainter p(this);
    p.save();
    if (hover) {
        QImage outline = img;
        QPainter p2(&outline);
        p2.setCompositionMode(QPainter::CompositionMode_SourceIn);
        p2.fillRect(outline.rect(), Qt::yellow);
        p2.end();

        p.setOpacity(0.8);
        p.drawImage(QPointF(0, 0), outline.scaled(width(), height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }
    p.drawImage(QPointF(rPad, rPad), img);
    p.restore();

    QLabel::paintEvent(event);
}
