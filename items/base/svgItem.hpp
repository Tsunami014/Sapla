#pragma once
#include <QGraphicsItem>
#include <QSvgRenderer>

class SvgGraphicItem : public QGraphicsItem {
public:
    SvgGraphicItem(const QString& svgPath);
    QRectF boundingRect() const override;
    void setRect(const QRectF& newRect);
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

private:
    QSvgRenderer renderer;
    QRectF rect{0, 0, 800, 600}; // default size
};
