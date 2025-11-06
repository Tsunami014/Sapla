#include "listWid.hpp"
#include <QPaintEvent>
#include <QPainter>
#include <QHeaderView>

ListWidget::ListWidget(QWidget *parent) : QTreeWidget(parent) {
    setSortingEnabled(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setAttribute(Qt::WA_TranslucentBackground);
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

