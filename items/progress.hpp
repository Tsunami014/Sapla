#pragma once
#include "base/myItem.hpp"

class ProgressBarItem : public MyGraphicsItem {
public:
    ProgressBarItem(QGraphicsItem* parent = nullptr);
    void setValue(double value);
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;
protected:
    double value;
};
