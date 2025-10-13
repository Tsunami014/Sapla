#pragma once
#include <QFontMetricsF>
#include <QList>
#include <QFontDatabase>  // new

inline QFont getFont(qreal targetH) {
    //QFont font;
    //font.setFamilies({"DejaVu Sans", "Ubuntu", "Segoe UI", "Arial", "Helvetica"});
    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);  // new
    QFontMetricsF metrics(font);
    qreal currentH = metrics.height();
    font.setPointSizeF(font.pointSizeF() * (targetH / currentH));
    return font;
}
