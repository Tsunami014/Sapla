#pragma once
#include "base/svgItem.hpp"
#include "gridLayout.hpp"
#include "sideRends.hpp"

class CardGraphicItem : public SvgGraphicItem {
public:
    CardGraphicItem(layout lay, BaseSideRend* front, BaseSideRend* back, QGraphicsItem* parent = nullptr);   
    ~CardGraphicItem();

    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;
    layout lay;
    uint8_t side;  // 0 if front, 255 if back, anywhere else for a transition effect between the 2
protected:
    BaseSideRend* front;
    BaseSideRend* back;
};

