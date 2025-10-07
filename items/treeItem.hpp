#pragma once
#include "base/svgItem.hpp"

class Tree : public SvgGraphicItem {
public:
    Tree(QGraphicsItem* parent = nullptr);
    ~Tree();
    void nextPhase();
    void setRect(const QRectF& newRect) override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

protected:
    static QSvgRenderer* baseRend;
    QRectF treeR;

private:
    int phase;
    bool isSmall;
};
