#pragma once
#include "rectIt.hpp"
#include <QSvgRenderer>

class SvgGraphicItem : public RectItem {
public:
    SvgGraphicItem(const QString& svgPath, QGraphicsItem* parent = nullptr);
    ~SvgGraphicItem();
    void setSvg(const QString& svgPath);
    bool contains(const QPointF& point) const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

protected:
    SvgGraphicItem(QGraphicsItem* parent = nullptr);
    QSvgRenderer* renderer;
};
