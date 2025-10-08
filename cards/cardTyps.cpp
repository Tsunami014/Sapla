#include "cardTyps.hpp"
#include "../items/cardLayouts.hpp"

bool BaseCardTyp::operator==(const BaseCardTyp& other) const {
    auto* it1 = getItem();
    auto* it2 = other.getItem();
    bool eq = *it1 == *it2;
    delete it1;
    delete it2;
    return eq;
}
bool BaseCardTyp::operator==(const CardGraphicItem& other) const {
    const auto* it = getItem();
    bool eq = *it == other;
    delete it;
    return eq;
}

TextCard::TextCard(QString fr, QString bk) : front(fr), back(bk) {}
CardGraphicItem* TextCard::getItem() const {
    return new CardGraphicItem(Single, new TextSide(front), new TextSide(back));
}
QString TextCard::getName() {
    return front;
}

