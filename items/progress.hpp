#pragma once
#include "../base/rectIt.hpp"

class ProgressBarItem : public RectItem {
public:
    ProgressBarItem(QGraphicsItem* parent = nullptr);
    void setValue(double value);
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;
    QPainterPath shape() const override;
protected:
    double value;
};
