#include "game.hpp"
#include "base/svgRend.hpp"

static QString rgbaFromHex(const QString& hex, double a) {
    QColor c(hex);
    return QString("rgba(%1,%2,%3,%4)")
        .arg(c.red()).arg(c.green()).arg(c.blue()).arg(QString::number(a,'f',2));
}

void MainGame::initStyles() {
    setStyleSheet(QString(
        "color: %1;"
        "QTextEdit {"
            "color: white;"
            "background: %4;"
            "border-radius: 6px;"
            "border: 1px solid #666;"
            "margin: 4px;"
            "padding: 4px;"
        "}"
        "QTextEdit:focus {"
            "border: 2px solid #333;"
        "}"
        "QTreeWidget::item { background: %5; }"
        "QTreeWidget {"
            "color: %7;"
            "border: 1px solid #333;"
            "padding: 4px;"
        "}"
        "QTreeWidget::item:selected {"
            "background-color: %3;"
        "}"
        "QTreeWidget::item:hover {"
            "background-color: %2;"
        "}"
        "QHeaderView::section {"
            "background-color: %6;"
            "border-top-left-radius: 10px;"
            "border-top-right-radius: 10px;"
            "padding: 4px;"
        "}"
        "QMenuBar {"
            "background: %1;"
        "}"
        "QMenuBar::item {"
            "padding: 4px;"
            "background: %1;"
            "border-radius: 6px;"
        "}"
        "QMenuBar::item:selected {"
            "background: %3;"
        "}"
        "QMenu {"
            "background: %2;"
            "border-radius: 6px;"
        "}"
        "QMenu::item {"
            "border-radius: 4px;"
            "padding: 4px;"
        "}"
        "QMenu::item:selected {"
            "background: rgba(255,255,255,30);"
        "}"
        ).arg(getCol("alight", -5))         // %1
         .arg(getCol("alight", -5, -5, 20)) // %2
         .arg(getCol("alight", 10))         // %3
         .arg(rgbaFromHex(getCol("alight", 0, -8), 0.86)) // %4
         .arg(getCol("adark",  -10))        // %5
         .arg(getCol("adark"))              // %6
         .arg(getCol("adark",  -15))        // %7
    );
    cols.ListWid = getCol("alight", -20);
}
