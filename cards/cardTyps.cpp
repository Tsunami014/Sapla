#include "cardTyps.hpp"
#include "formElms.hpp"
#include "getCards.hpp"
#include "../items/cardLayouts.hpp"

void registerCardTypes() {
    REGISTER_CARD(TextCard, "Text card")
    REGISTER_CARD(DoubleSidedCard, "Text card and reverse")
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

/////// DoubleSidedCard

QString SideXtra::fullTxt(bool front) const {
    if (!front) return txt;
    return prefix + txt + suffix;
}
DoubleSidedCard::DoubleSidedCard(SideXtra fr, SideXtra bk) : front(fr), back(bk) {}
BaseCardTyp* DoubleSidedCard::newBlank() { return new DoubleSidedCard({"", "", ""}, {"", "", ""}); }
CardGraphicItem* DoubleSidedCard::getItem() const {
    return new CardGraphicItem(Single, new TextSide(front.fullTxt(true)), new TextSide(back.fullTxt(false)));
}
QString DoubleSidedCard::getName() {
    return front.fullTxt(true);
}
void DoubleSidedCard::createForm(QVBoxLayout* lay, QTreeWidgetItem* trit) {
    Form::labelField(lay, "Front:");
    Form::textXtraField(lay, front, [=](int side, const QString &s){
        if (side == 0) front.prefix = s;
        else if (side == 1) front.txt = s;
        else if (side == 2) front.suffix = s;
        trit->setText(0, getName());
        writeCards();
    });
    Form::labelField(lay, "Back:");
    Form::textXtraField(lay, back, [=](int side, const QString &s){
        if (side == 0) back.prefix = s;
        else if (side == 1) back.txt = s;
        else if (side == 2) back.suffix = s;
        writeCards();
    });
}
bool DoubleSidedCard::canParse(const QString& header) {
    return header == "d";
}
SideXtra parseSide(QString& txt) {
    int pos1 = txt.indexOf('|');
    if (pos1 == -1) qFatal() << "Double sided card side needs 3 delimitered parts, found 0 delimiters!";
    int pos2 = txt.indexOf('|', pos1 + 1);
    if (pos2 == -1) qFatal() << "Double sided card side needs 3 delimitered parts, only found 1 delimiter!";

    QString pref = txt.left(pos1);
    QString mid  = txt.mid(pos1 + 1, pos2 - pos1 - 1);
    QString suff = txt.mid(pos2 + 1);
    return {unSafe(pref), unSafe(mid), unSafe(suff)};
}
BaseCardTyp* DoubleSidedCard::parse(const QString& header, QTextStream& in) {
    QString frtxt = tryReadLine(in, "DoubleSidedCard needs a front, but not provided!");
    QString bktxt = tryReadLine(in, "DoubleSidedCard needs a back, but not provided!");
    return new DoubleSidedCard(parseSide(frtxt), parseSide(bktxt));
}
void DoubleSidedCard::toFile(QTextStream& out) {
    out << "d\n"
        << makeSafe(front.prefix) << "|" << makeSafe(front.txt) << "|" << makeSafe(front.suffix) << "\n"
        << makeSafe(back.prefix) << "|" << makeSafe(back.txt) << "|" << makeSafe(back.suffix) << "\n";
}

