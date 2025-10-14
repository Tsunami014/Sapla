#include "cardTyps.hpp"
#include "formElms.hpp"
#include "getCards.hpp"
#include "../items/cardLayouts.hpp"

void registerCardTypes() {
    REGISTER_CARD(TextCard, "Text card")
}

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

/////// TextCard

TextCard::TextCard(QString fr, QString bk) : front(fr), back(bk) {}
BaseCardTyp* TextCard::newBlank() { return new TextCard("", ""); }
CardGraphicItem* TextCard::getItem() const {
    return new CardGraphicItem(Single, new TextSide(front), new TextSide(back));
}
QString TextCard::getName() {
    return front;
}
void TextCard::createForm(QVBoxLayout* lay, QTreeWidgetItem* trit) {
    Form::labelField(lay, "Front:");
    Form::textField(lay, front, [=](const QString &s){
        front = s;
        trit->setText(0, s);
        writeCards();
    });
    Form::labelField(lay, "Back:");
    Form::textField(lay, back, [=](const QString &s){
        back = s;
        writeCards();
    });
}
bool TextCard::canParse(const QString& header) {
    return header == "t";
}
BaseCardTyp* TextCard::parse(const QString& header, QTextStream& in) {
    QString front = tryReadLine(in, "TextCard needs a front, but not provided!");
    QString back = tryReadLine(in, "TextCard needs a back, but not provided!");
    return new TextCard(unSafe(front), unSafe(back));
}
void TextCard::toFile(QTextStream& out) {
    out << "t\n" << makeSafe(front) << "\n" << makeSafe(back) << "\n";
}

