#pragma once
#include <QTreeWidget>

class CardTree : public QTreeWidget {
    Q_OBJECT
public:
    CardTree(QWidget *parent = nullptr);
protected:
    bool viewportEvent(QEvent *event) override;
};

