#pragma once
#include "../base/rectIt.hpp"
#include "progress.hpp"
#include <QSvgRenderer>

QByteArray rendTreePhase(int phase);

class Tree : public RectItem {
public:
    Tree(QGraphicsItem* parent = nullptr);
    bool grow(double amount);
    bool isDone();
    void nextPhase();
    void setRect(const QRectF& newRect) override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

protected:
    double growth;
    double toNext;
    ProgressBarItem pb;
    static QSvgRenderer* baseRend;
    QSvgRenderer* treeRend;

private:
    int phase;
    bool isSmall;
};
