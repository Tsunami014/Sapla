#pragma once
#include <QGraphicsItem>
#include <QSvgRenderer>

class SvgGraphicItem : public QGraphicsItem {
public:
    SvgGraphicItem(const QString& svgPath, QGraphicsItem* parent = nullptr);
    ~SvgGraphicItem();
    void setRect(const QRectF& newRect);
    bool contains(const QPointF& point) const override;
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

protected:
    QRectF rect{0, 0, 800, 600}; // default size, will be overridden
    QSvgRenderer* renderer;

private:
    bool _contains(const QPointF &point);
};
