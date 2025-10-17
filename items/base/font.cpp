#include "font.hpp"
#include <QApplication>
#include <QFontMetricsF>
#include <QList>

QFont getFont() {
    QFont font = QApplication::font();
    font.setStyle(QFont::StyleNormal);
    font.setWeight(QFont::Normal);
    return font;
}
QFont getFont(qreal targetH) {
    QFont font = getFont();
    QFontMetricsF metrics(font);
    qreal currentH = metrics.height();
    font.setPointSizeF(font.pointSizeF() * (targetH / currentH));
    return font;
}

