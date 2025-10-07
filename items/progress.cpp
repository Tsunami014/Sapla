#include "progress.hpp"
#include <QPainter>

ProgressBarItem::ProgressBarItem(QGraphicsItem* parent) : MyGraphicsItem(parent) {
    value = 0;
}

void ProgressBarItem::setValue(double v) {
    v = std::clamp(v, 0.0, 1.0);
    if (qFuzzyCompare(value, v)) return;
    value = v;
    update();
}

void ProgressBarItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    QLinearGradient gradient(rect.left(), 0, rect.right(), 0);
    const double offs = 0.02;
    qreal start = qMax(value-offs, 0.0);
    gradient.setColorAt(start, QColor(50, 205, 50));
    gradient.setColorAt(qMin(start+offs*2, 1.0), QColor(0, 0, 0, 0));

    painter->setBrush(QBrush(gradient));
    painter->drawRect(rect);
}
