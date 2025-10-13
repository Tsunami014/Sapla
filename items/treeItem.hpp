#pragma once
#include "base/svgItem.hpp"
#include "progress.hpp"

class Tree : public SvgGraphicItem {
public:
    Tree(QGraphicsItem* parent = nullptr);
    void updateCipher() { update(); }  // new
    void changeTxt();  // new
    bool grow(double amount);
    void nextPhase();
    void lastPhase();  // Does not update toNext correctly
    void setRect(const QRectF& newRect) override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

protected:
    static QString fullTxt;  // new
    double growth;
    double toNext;
    ProgressBarItem pb;
    static QSvgRenderer* baseRend;

private:
    int phase;
    bool isSmall;
};
