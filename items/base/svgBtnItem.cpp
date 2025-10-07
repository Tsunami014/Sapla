#include "svgBtnItem.hpp"
#include <QPainter>

SvgBtnItem::SvgBtnItem(const QString& svgPath, QGraphicsItem* parent) : SvgGraphicItem(svgPath, parent) {
    txt = new QGraphicsTextItem(this);
}
SvgBtnItem::SvgBtnItem(const QString& svgPath, const QString& text, QGraphicsItem* parent) : SvgGraphicItem(svgPath, parent) {
    txt = new QGraphicsTextItem(text, this);
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

