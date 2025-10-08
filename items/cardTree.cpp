#include "cardTree.hpp"
#include <QPaintEvent>
#include <QPainter>
#include <QHeaderView>

CardTree::CardTree(QWidget *parent) : QTreeWidget(parent) {
    setSortingEnabled(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    header()->setSectionResizeMode(QHeaderView::Stretch);
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet(
        "QTreeWidget::item { background: transparent; }"
        "QTreeWidget {"
            "color: black;"
            "border: 1px solid #333;"
        "}"
        "QTreeWidget::item:selected {"
            "background-color: rgba(80, 80, 80, 200);"
        "}"
        "QTreeWidget::item:hover { background-color: rgba(120, 120, 120, 140); }"
        "QHeaderView::section { padding: 4px; }"
    );
    viewport()->setAutoFillBackground(false);
}

bool CardTree::viewportEvent(QEvent *event) {
    if (event->type() == QEvent::Paint) {
        QPaintEvent *pe = static_cast<QPaintEvent*>(event);
        QPainter p(viewport());
        p.fillRect(pe->rect(), QColor(128, 128, 128, 128));
    }
    return QTreeWidget::viewportEvent(event);
}

