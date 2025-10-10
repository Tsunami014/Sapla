#include "progress.hpp"
#include <QPainter>

ProgressBarItem::ProgressBarItem(QGraphicsItem* parent) : RectItem(parent) {
    setZValue(2);
    value = 0;
}

void ProgressBarItem::setValue(double v) {
    v = std::clamp(v, 0.0, 1.0);
    if (qFuzzyCompare(value, v)) return;
    value = v;
    update();
}

QPainterPath ProgressBarItem::shape() const {
    QPainterPath path;
    path.addRoundedRect(rect, 10, 10);
    return path;
}

void ProgressBarItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    QLinearGradient gradient(rect.left(), 0, rect.right(), 0);
    const double offs = 0.02;
    qreal start = qMax(value-offs, 0.0);
    qreal end = qMin(start+offs*2, 1.0);
    if (start != 0) gradient.setColorAt(start, QColor(50, 205, 50));
    if (end != 1)   gradient.setColorAt(end,   QColor(235, 220, 200));

    auto path = shape();
    painter->fillPath(path, gradient);
    painter->setPen(QPen(QBrush(QColor(75, 35, 5)), 2));
    painter->drawPath(path);
}
