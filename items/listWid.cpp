#include "listWid.hpp"
#include <QPaintEvent>
#include <QPainter>
#include <QHeaderView>

ListWidget::ListWidget(QWidget *parent) : QTreeWidget(parent) {
    setSortingEnabled(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    header()->setSectionResizeMode(QHeaderView::Stretch);
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet(
        "QTreeWidget::item { background: rgb(193, 154, 107); }"
        "QTreeWidget {"
            "color: rgb(70, 35, 10);"
            "border: 1px solid #333;"
            "padding: 4px;"
        "}"
        "QTreeWidget::item:selected {"
            "background-color: rgba(210, 125, 45, 200);"
        "}"
        "QTreeWidget::item:hover {"
            "background-color: rgba(185, 115, 50, 140);"
        "}"
        "QHeaderView::section {"
            "background-color: rgb(90, 40, 10);"
            "border-top-left-radius: 10px;"
            "border-top-right-radius: 10px;"
            "padding: 4px;"
        "}"
    );
    viewport()->setAutoFillBackground(false);
}

bool ListWidget::viewportEvent(QEvent *event) {
    if (event->type() == QEvent::Paint) {
        QPaintEvent *pe = static_cast<QPaintEvent*>(event);
        QPainter p(viewport());
        p.fillRect(pe->rect(), QColor(140, 68, 44, 128));
    }
    return QTreeWidget::viewportEvent(event);
}

