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

constexpr int wpad = 30;
constexpr int hpad = 0;
QSize SvgBtn::sizeHint() const {
    QSize base = QLabel::sizeHint();
    return QSize(base.width() + wpad * 2, base.height() + hpad * 2);
}

void SvgBtn::setSvg(const QString& path) {
    rend->load(RenderSvg(path));
}

void SvgBtn::paintEvent(QPaintEvent* event) {
    constexpr int rPad = 5;
    QRectF r1 = rect().adjusted(rPad, rPad, -rPad, -rPad);
    QImage img(r1.width(), r1.height(), QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::transparent);
    QPainter p1(&img);
    rend->render(&p1, QRectF(QPointF(0, 0), r1.size()));
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

    QRect r2 = contentsRect();
    r2.adjust(wpad, hpad, -wpad, -hpad);

    QTextOption option;
    option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    option.setAlignment(alignment());

    p.setPen(palette().color(foregroundRole()));
    p.drawText(r2, text(), option);
}
