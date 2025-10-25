#include "svgBtnItem.hpp"
#include <QPainter>
#include <QGraphicsSceneHoverEvent>
#include <QApplication>
#include <QCursor>

SvgBtnItem::SvgBtnItem(const QString &svgPath, QGraphicsItem *parent) : SvgGraphicItem(svgPath, parent) {
    hovering = false;
    setAcceptHoverEvents(true);
}

QRectF SvgBtnItem::boundingRect() const {
    QRectF newR = rect;
    newR.setX(rect.x() - rect.width()*0.1);
    newR.setWidth(rect.width()*1.2);
    newR.setY(rect.y() - rect.height()*0.1);
    newR.setHeight(rect.height()*1.2);
    return newR;
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

void SvgBtnItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (contains(event->pos())) {
        if (onClick) onClick();
    }
}

void SvgBtnItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    QRectF target = rect;
    if (hovering) {
        target = boundingRect();
    }
    renderer->render(painter, target);
}



TxtBtnItem::TxtBtnItem(const QString& svgPath, QGraphicsItem* parent) : SvgBtnItem(svgPath, parent) {
    txt = new MarkdownGraphicsText(this);
    txt->setAcceptHoverEvents(false);
}
TxtBtnItem::TxtBtnItem(const QString& svgPath, const QString& text, QGraphicsItem* parent) : SvgBtnItem(svgPath, parent) {
    txt = new MarkdownGraphicsText(text, this);
    txt->setAcceptHoverEvents(false);
}

void TxtBtnItem::setRect(const QRectF& newRect) {
    QRectF txtBR = txt->boundingRect();
    qreal xtraX = txtBR.width() * 0.2;
    qreal xtraY = txtBR.height() * 0.1;
    SvgGraphicItem::setRect(QRectF(
        newRect.x(), newRect.y(), txtBR.width() + xtraX, txtBR.height() + xtraY
    ));
    txt->setPos(newRect.x() + xtraX/2, newRect.y());
}

void TxtBtnItem::setTxtWid(qreal wid) {
    txt->setTextWidth(wid);
    setRect(rect);
}
void TxtBtnItem::setTxtColour(const QColor& colour) {
    txt->setDefaultTextColor(colour);
}
void TxtBtnItem::setPos(const QPointF& pos) {
    SvgGraphicItem::setRect(QRectF(pos, rect.size()));
    txt->setPos(pos.x() + (txt->boundingRect().width()*0.2)/2, pos.y());
}
void TxtBtnItem::setText(const QString& text) {
    txt->setMarkdown(text);
    setRect(rect);
}
void TxtBtnItem::setFont(const QFont& font) {
    txt->setFont(font);
    setRect(rect);
}

