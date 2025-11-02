#include "svgBtn.hpp"
#include "../base/svgRend.hpp"
#include <QPainter>

SvgBtn::SvgBtn(QWidget* parnt)
    : QLabel(parnt), rend(new QSvgRenderer(this)) {
        init();
    }
SvgBtn::SvgBtn(const QString& pth, QWidget* parnt)
    : QLabel(parnt), rend(new QSvgRenderer(RenderSvg(pth), this)) {
        init();
    }
void SvgBtn::init() {
    setWordWrap(true);
    setAlignment(Qt::AlignCenter);
    setMouseTracking(true);
    setContentsMargins(40, 5, 40, 5);
}

void SvgBtn::enterEvent(QEnterEvent* event) {
    hover = true;
    update();
}
void SvgBtn::leaveEvent(QEvent* event) {
    hover = false;
    update();
}
void SvgBtn::mousePressEvent(QMouseEvent* event) {
    emit clicked();
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
