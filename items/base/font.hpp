#pragma once
#include <QFontMetricsF>
#include <QList>

inline QFont getFont(qreal targetH) {
    QFont font;
    font.setFamilies({"DejaVu Sans", "Ubuntu", "Segoe UI", "Arial", "Helvetica"});
    QFontMetricsF metrics(font);
    qreal currentH = metrics.height();
    font.setPointSizeF(font.pointSizeF() * (targetH / currentH));
    return font;
}
