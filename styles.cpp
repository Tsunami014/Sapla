#include "game.hpp"
#include "base/svgRend.hpp"
#include "base/font.hpp"
#include <QDialog>

static QString rgba(const QColor& c, double a) {
    return QString("rgba(%1,%2,%3,%4)")
        .arg(c.red()).arg(c.green()).arg(c.blue()).arg(std::clamp(int(std::round(a * 255)), 0, 255));
}

void MainGame::initStyles() {
    QString globals = QString(
        "color: %1;"
    )
        .arg(getCol("alight", 100, 150, 10)) // %1 - color
    ;
    QString treeWidAndMenu = QString(
        "QTreeWidget::item { background: %1; }"
        "QTreeWidget {"
            "border: 1px solid #333;"
            "padding: 4px;"
        "}"
        "QTreeWidget::item:hover { background-color: %2; }"
        "QTreeWidget::item:selected { background-color: %3; }"
        "QHeaderView::section {"
            "background-color: %4;"
            "border: 1px solid #666;"
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
    )   // QTreeWidget/some of QMenuBar
         .arg(getCol("alight", -20, -10)) // %1 - item
         .arg(getCol("alight", -10, -5))  // %2 - item hover
         .arg(getCol("alight", 0, 5))   // %3 - item select
         .arg(getCol("adark", -10, 10)) // %4 - header
        // QMenuBar overrides
         .arg(getCol("adark", 50, 20))  // %5 - item
         .arg(getCol("adark", 30, 10, 10)) // %6 - header
    ;
    QString textEdit = QString(
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
        "QLabel { color: %2; }"
    )
         .arg(rgba(getQCol("alight", 0, -18), 0.86)) // %1 - QTextEdit bg
         .arg(getCol("adark", -100, 70))  // %2 - QLabel text colour
    ;
    QString misc = QString(
        "QSplitter::handle {"
            "background-color: %1;"
            "border-radius: 6px;"
        "}"
    )
        .arg(getCol("alight", -100, 0, 20))  // %1 - QSplitter handle colour
    ;
    setStyleSheet(
        "QWidget {" + globals + "}" +
        treeWidAndMenu +
        textEdit +
        misc
    );

    styls.dialogStyl = QString(
        "background-color: %1;"
        "color: %2;"
        "border-radius: 6px;"
    )
        .arg(getCol("alight", -200, 111)) // %1 - bg colour
        .arg(getCol("dark", 100, -70)) // %2 - text colour
    ;
    styls.listWidCol = getCol("alight", -42, 10);
    styls.logBgCol = getCol("shadow", -200, 20);
    styls.mdHlCol = getCol("light", 20, -60);

    setFont(getFont(1.2));
}
void MainGame::styliseDialog(QDialog* dialog, const QString& xtraStyl) {
    dialog->setStyleSheet(styls.dialogStyl + xtraStyl);
    dialog->setFont(font());
}
