#include "svgBtnItem.hpp"
#include <QPainter>
#include <QGraphicsSceneHoverEvent>
#include <QCursor>

SvgBtnItem::SvgBtnItem(const QString &svgPath, QGraphicsItem *parent) : SvgGraphicItem(svgPath, parent) {
    txt = new QGraphicsTextItem(this);
    init();
}
SvgBtnItem::SvgBtnItem(const QString& svgPath, const QString& text, QGraphicsItem* parent) : SvgGraphicItem(svgPath, parent) {
    txt = new QGraphicsTextItem(text, this);
    init();
}
void SvgBtnItem::init() {
    hovering = false;
    setAcceptHoverEvents(true);
    txt->setAcceptHoverEvents(false);
}

QRectF SvgBtnItem::boundingRect() const {
    QRectF newR = rect;
    newR.setX(rect.x() - rect.width()*0.1);
    newR.setWidth(rect.width()*1.2);
    newR.setY(rect.y() - rect.height()*0.1);
    newR.setHeight(rect.height()*1.2);
    return newR;
}
bool SvgBtnItem::contains(const QPointF& point) const {
    return SvgGraphicItem::contains(point);
}

void SvgBtnItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
    bool oldHover = hovering;
    if (contains(event->pos())) {
        hovering = true;
        setCursor(Qt::PointingHandCursor);
    } else {
        if (hovering) {
            unsetCursor();
        }
        hovering = false;
    }
    if (oldHover != hovering) { update(); }
    SvgGraphicItem::hoverMoveEvent(event);
}
void SvgBtnItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    hovering = false;
    unsetCursor();
    update();
    SvgGraphicItem::hoverLeaveEvent(event);
}

void SvgBtnItem::setTxtColour(const QColor& colour) {
    txt->setDefaultTextColor(colour);
}
void SvgBtnItem::setRect(const QRectF& newRect) {
    QRectF txtBR = txt->boundingRect();
    qreal xtraX = txtBR.width() * 0.2;
    qreal xtraY = txtBR.height() * 0.1;
    MyGraphicsItem::setRect(QRectF(
        newRect.x(), newRect.y(), txtBR.width() + xtraX, txtBR.height() + xtraY
    ));
    txt->setPos(newRect.x() + xtraX/2, newRect.y());
}
void SvgBtnItem::setPos(const QPointF& pos) {
    MyGraphicsItem::setRect(QRectF(pos, rect.size()));
    txt->setPos(pos.x() + (txt->boundingRect().width()*0.2)/2, pos.y());
}
void SvgBtnItem::setText(const QString& text) {
    txt->setPlainText(text);
    setRect(rect);
}
QString SvgBtnItem::getText() {
    return txt->toPlainText();
}
void SvgBtnItem::setFont(const QFont& font) {
    txt->setFont(font);
    setRect(rect);
}

void SvgBtnItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    QRectF target = rect;
    if (hovering) {
        target = boundingRect();
    }
    renderer->render(painter, target);
}

