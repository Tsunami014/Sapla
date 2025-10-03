#include "cardIt.hpp"
#include "gridLayout.hpp"

CardGraphicItem::CardGraphicItem(layout l, BaseSideRend* fr, BaseSideRend* bk, QGraphicsItem* parent) : SvgGraphicItem(l.fname, parent) {
    lay = l;
    front = fr;
    back = bk;
    side = 0;
}

CardGraphicItem::~CardGraphicItem() {
    delete front;
    delete back;
}

void CardGraphicItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* sogi, QWidget* w) {
    SvgGraphicItem::paint(painter, sogi, w);
    // TODO: Partial flipped states
    if (side == 0) {
        front->render(painter, rect);
    } else {
        back->render(painter, rect);
    }
}
