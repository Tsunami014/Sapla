#include "game.hpp"
#include "base/svgRend.hpp"
#include "base/font.hpp"

static QString rgba(const QColor& c, double a) {
    return QString("rgba(%1,%2,%3,%4)")
        .arg(c.red()).arg(c.green()).arg(c.blue()).arg(std::clamp(int(std::round(a * 255)), 0, 255));
}

void MainGame::initStyles() {
    setStyleSheet(QString(
        "QTreeWidget::item { background: %1; }"
        "QTreeWidget {"
            "border: 1px solid #333;"
            "padding: 4px;"
        "}"
        "QTreeWidget::item:hover { background-color: %2; }"
        "QTreeWidget::item:selected { background-color: %3; }"
        "QHeaderView::section {"
            "background-color: %4;"
            "padding: 4px;"
        "}"
        "QHeaderView::section:first {"
            "border-top-left-radius: 10px;"
        "}"
        "QHeaderView::section:last {"
            "border-top-right-radius: 10px;"
        "}"
        "QMenuBar {"
            "background: %6;"
        "}"
        "QMenuBar::item {"
            "padding: 4px;"
            "background: %5;"
            "border-radius: 6px;"
        "}"
        "QMenuBar::item:hover { background: %2; }"
        "QMenuBar::item:selected { background: %3; }"
        "QMenu {"
            "background: %1;"
            "border-radius: 6px;"
        "}"
        "QMenu::item {"
            "background: %1;"
            "border-radius: 4px;"
            "padding: 4px;"
        "}"
        "QMenu::item:hover { background: %2; }"
        "QMenu::item:selected { background: %3; }"

        "QTextEdit {"
            "color: white;"
            "background-color: %7;"
            "border-radius: 6px;"
            "border: 1px solid #666;"
            "margin: 4px;"
            "padding: 4px;"
        "}"
        "QTextEdit:focus {"
            "border: 2px solid #333;"
        "}"
        "QLabel { color: %8; }"
        )
         // QTreeWidget/some of QMenuBar
         .arg(getCol("alight", -20, -10)) // %1 - item
         .arg(getCol("alight", -10, -5))  // %2 - item hover
         .arg(getCol("alight", 0, 5))   // %3 - item select
         .arg(getCol("adark", -10, 10)) // %4 - header
         // QMenuBar overrides
         .arg(getCol("adark", 50, 20))  // %5 - item
         .arg(getCol("adark", 30, 10, 10)) // %6 - header
         // Misc
         .arg(rgba(getQCol("alight", 0, -18), 0.86)) // %7 - QTextEdit bg
         .arg(getCol("adark", -100, 70))  // %8 - QLabel text colour
    );
    QColor txtCol = getQCol("alight", 100, 150, 10);
    QPalette palette = qApp->palette();
    palette.setColor(QPalette::WindowText, txtCol);
    palette.setColor(QPalette::Text, txtCol);
    qApp->setPalette(palette);
    cols.ListWid = getCol("alight", -42, 10);
    qApp->setFont(getFont(1.5));
}
