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

QRectF ProgressBarItem::getPBrect() {
    return QRectF(
        rect.x() + rect.width()*0.1,
        rect.y() + rect.height()*0.92,
        rect.width()*0.8,
        rect.height()*0.06
    );
}

void ProgressBarItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    QRectF pbR = getPBrect();

    QLinearGradient gradient(pbR.left(), 0, pbR.right(), 0);
    const double offs = 0.02;
    qreal start = qMax(value-offs, 0.0);
    gradient.setColorAt(start, QColor(50, 205, 50));
    gradient.setColorAt(qMin(start+offs*2, 1.0), QColor(0, 0, 0, 0));

    painter->setBrush(QBrush(gradient));
    painter->drawRect(pbR);
}
