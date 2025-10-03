#pragma once
#include <QGraphicsItem>
#include <QSvgRenderer>

class SvgGraphicItem : public QGraphicsItem {
public:
    SvgGraphicItem(const QString& svgPath, QGraphicsItem* parent = nullptr);
    void setRect(const QRectF& newRect);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

protected:
    QRectF rect{0, 0, 800, 600}; // default size, will be overridden

private:
    QSvgRenderer renderer;
};
