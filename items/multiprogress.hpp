#pragma once
#include "../base/rectIt.hpp"

class MultiProgressBarItem : public RectItem {
public:
    MultiProgressBarItem(QGraphicsItem* parent = nullptr);
    void setValues(std::vector<float> vs);
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;
    QPainterPath shape() const override;
protected:
    std::vector<double> values;
};
