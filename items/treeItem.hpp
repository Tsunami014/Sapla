#pragma once
#include "base/svgItem.hpp"
#include "progress.hpp"

class Tree : public SvgGraphicItem {
public:
    Tree(QGraphicsItem* parent = nullptr);
    ~Tree();
    bool grow(double amount);
    void nextPhase();
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
