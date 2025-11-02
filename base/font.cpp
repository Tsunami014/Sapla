#include "font.hpp"
#include <QApplication>
#include <QFontMetricsF>
#include <QList>

QFont getFont(double scale) {
    QFont font = QApplication::font();
    font.setStyle(QFont::StyleNormal);
    font.setWeight(QFont::Normal);
    font.setPointSizeF(font.pointSizeF() * scale);
    return font;
}
QFont getFontSpan(qreal targetH) {
    QFont font = getFont(1);
    QFontMetricsF metrics(font);
    qreal currentH = metrics.height();
    font.setPointSizeF(font.pointSizeF() * (targetH / currentH));
    return font;
}

