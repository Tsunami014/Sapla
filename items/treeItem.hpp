#pragma once
#include "base/svgItem.hpp"
#include "progress.hpp"

class Tree : public SvgGraphicItem {
public:
    Tree(QGraphicsItem* parent = nullptr);
    bool grow(double amount);
    void nextPhase();
    void lastPhase();  // Does not update toNext correctly
    void setRect(const QRectF& newRect) override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

protected:
    double growth;
    double toNext;
    ProgressBarItem pb;
    static QSvgRenderer* baseRend;
    QRectF treeR;

private:
    int phase;
    bool isSmall;
};
