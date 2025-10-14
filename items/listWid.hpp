#pragma once
#include <QTreeWidget>

class ListWidget : public QTreeWidget {
    Q_OBJECT
public:
    ListWidget(QWidget *parent = nullptr);
protected:
    bool viewportEvent(QEvent *event) override;
};

