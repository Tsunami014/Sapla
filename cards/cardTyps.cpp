#include "cardTyps.hpp"
#include "../items/cardLayouts.hpp"

TextCard::TextCard(QString fr, QString bk) : front(fr), back(bk) {}
CardGraphicItem* TextCard::getItem() {
    return new CardGraphicItem(Single, new TextSide(front), new TextSide(back));
}

