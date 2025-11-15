#include "game.hpp"
#include "base/svgRend.hpp"

void MainGame::initStyles() {
    setStyleSheet(QString(
        "color: %1;"
        "QTextEdit {"
            "color: white;"
            "background: %4DC;"
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
            "background: %4;"
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
        ).arg(getCol("alight", {-5, -5, -5}))
         .arg(getCol("alight", {-12, 12, 12}, 20))  // Different (and more greyish)
         .arg(getCol("alight", {18, 18, 18}))  // A bit lighter
         .arg(getCol("alight", {-15, -15, -15}))  // A bit darker
         .arg(getCol("alight", {-30, -30, -30}))
         .arg(getCol("adark",  {0, 0, 0}))
         .arg(getCol("adark",  {-22, -22, -15}))  // Darker
    );
    cols.ListWid = getCol("alight", {-18, -30, 0});
}
