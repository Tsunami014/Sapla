#pragma once
#include "myItem.hpp"
#include <QSvgRenderer>

class SvgGraphicItem : public MyGraphicsItem {
public:
    SvgGraphicItem(const QString& svgPath, QGraphicsItem* parent = nullptr);
    ~SvgGraphicItem();
    bool contains(const QPointF& point) const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

protected:
    SvgGraphicItem(QGraphicsItem* parent = nullptr);
    QSvgRenderer* renderer;
};
