#include "svgBtn.hpp"
#include <QPainter>
#include <QMouseEvent>

SvgBtn::SvgBtn(QWidget* parnt)
    : QLabel(parnt), SvgUtils() {
        init();
    }
SvgBtn::SvgBtn(const QString& pth, QWidget* parnt)
    : QLabel(parnt), SvgUtils(pth) {
        init();
    }
void SvgBtn::init() {
    setMouseTracking(true);
    setWordWrap(true);
    setAlignment(Qt::AlignCenter);
    setContentsMargins(40, 5, 40, 20);
    if (hover) {
        setCursor(Qt::PointingHandCursor);
    }
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
void SvgBtn::mousePressEvent(QMouseEvent* event) {
    if (touching(mapFromGlobal(QCursor::pos()))) {
        emit clicked();
    }
}

void SvgBtn::paintEvent(QPaintEvent* event) {
    constexpr int rPad = 5;
    QPainter p(this);
    paintSvg(&p, rect().adjusted(rPad, rPad, -rPad, -rPad));
    QLabel::paintEvent(event);
}
