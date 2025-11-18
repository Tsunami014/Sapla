#include "game.hpp"
#include "base/svgRend.hpp"
#include "base/font.hpp"

static QString rgba(const QColor& c, double a) {
    return QString("rgba(%1,%2,%3,%4)")
        .arg(c.red()).arg(c.green()).arg(c.blue()).arg(std::clamp(int(std::round(a * 255)), 0, 255));
}

void MainGame::initStyles() {
    setStyleSheet(QString(
        "QTextEdit {"
            "color: white;"
            "background-color: %1;"
            "border-radius: 6px;"
            "border: 1px solid #666;"
            "margin: 4px;"
            "padding: 4px;"
        "}"
        "QTextEdit:focus {"
            "border: 2px solid #333;"
        "}"
        "QTreeWidget::item { background: %2; }"
        "QTreeWidget {"
            "border: 1px solid #333;"
            "padding: 4px;"
        "}"
        "QTreeWidget::item:hover { background-color: %3; }"
        "QTreeWidget::item:selected { background-color: %4; }"
        "QHeaderView::section {"
            "background-color: %5;"
            "padding: 4px;"
        "}"
        "QHeaderView::section:first {"
            "border-top-left-radius: 10px;"
        "}"
        "QHeaderView::section:last {"
            "border-top-right-radius: 10px;"
        "}"
        "QMenuBar {"
            "background: %7;"
        "}"
        "QMenuBar::item {"
            "padding: 4px;"
            "background: %6;"
            "border-radius: 6px;"
        "}"
        "QMenuBar::item:hover { background: %3; }"
        "QMenuBar::item:selected { background: %4; }"
        "QMenu {"
            "background: %2;"
            "border-radius: 6px;"
        "}"
        "QMenu::item {"
            "background: %2;"
            "border-radius: 4px;"
            "padding: 4px;"
        "}"
        "QMenu::item:hover { background: %3; }"
        "QMenu::item:selected { background: %4; }"
        )
         // Text edit
         .arg(rgba(getQCol("alight", 0, -18), 0.86)) // %1 - bg
         // QTreeWidget/some of QMenuBar
         .arg(getCol("alight", -20, -10)) // %2 - item
         .arg(getCol("alight", -10, -5))  // %3 - item hover
         .arg(getCol("alight", 0, 5))   // %4 - item select
         .arg(getCol("adark", -10, 10)) // %5 - header
         // QMenuBar overrides
         .arg(getCol("adark", 50, 20))  // %6 - item
         .arg(getCol("adark", 30, 10, 10)) // %7 - header
    );
    QColor txtCol = getQCol("alight", 20, 32);
    QPalette palette = qApp->palette();
    palette.setColor(QPalette::WindowText, txtCol);
    palette.setColor(QPalette::Text, txtCol);
    qApp->setPalette(palette);
    cols.ListWid = getCol("alight", -40);
    qApp->setFont(getFont(1));
}
