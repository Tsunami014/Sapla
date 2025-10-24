#include "cardTyps.hpp"
#include "formElms.hpp"
#include "getCards.hpp"

void registerCardTypes() {
    REGISTER_CARD(TextCard)
    REGISTER_CARD(DoubleSidedCard)
}

bool FlashCard::operator==(const FlashCard& other) const {
    return other.card == card && other.idx == idx;
}

/////// TextCard

TextCard::TextCard(QString fr, QString bk) : front(fr), back(bk) {
    flashCs = {FlashCard{this, 0}};
}
BaseCardTyp* TextCard::newBlank() { return new TextCard("", ""); }
BaseSideRend* TextCard::getSide(const FlashCard& fc, int side) const {
    QString s;
    if (side == 1) { s = front; } else { s = back; }
    return new TextSide(s);
}
QString TextCard::getName() {
    QString frontCpy = front;
    return frontCpy.replace('\n', "  ");
}
void TextCard::createForm(QVBoxLayout* lay, QTreeWidgetItem* trit) {
    Form::labelField(lay, "Front:");
    Form::textField(lay, &front, [=](const QString &s){
        trit->setText(0, getName());
    });
    Form::labelField(lay, "Back:");
    Form::textField(lay, &back);
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
    if (!front) return bprefix + txt + bsuffix;
    return fprefix + txt + fsuffix;
}
QString SideXtra::forCard() const {
    return QString("%1|%2|%3|%4|%5")
        .arg(makeSafe(fprefix))
        .arg(makeSafe(bprefix))
        .arg(makeSafe(txt))
        .arg(makeSafe(fsuffix))
        .arg(makeSafe(bsuffix));
}
DoubleSidedCard::DoubleSidedCard(SideXtra fr, SideXtra bk) : front(fr), back(bk) {
    flashCs = { FlashCard{this, 0}, FlashCard{this, 1} };
}
BaseCardTyp* DoubleSidedCard::newBlank() { return new DoubleSidedCard({}, {}); }
BaseSideRend* DoubleSidedCard::getSide(const FlashCard& fc, int side) const {
    SideXtra s;
    if (side == 1) { s = front; } else { s = back; };
    if (fc.idx == 0) {
        return new TextSide(s.fullTxt(true));
    } else {
        return new TextSide(s.fullTxt(false));
    }
}
QString DoubleSidedCard::getName() {
    return front.fullTxt(true).replace('\n', "  ");
}
void DoubleSidedCard::createForm(QVBoxLayout* lay, QTreeWidgetItem* trit) {
    Form::labelField(lay, "Side 1:");
    Form::textXtraField(lay, &front, [=](int side, const QString &s){
        trit->setText(0, getName());
    });
    Form::spacing(lay, 40);
    Form::labelField(lay, "Side 2:");
    Form::textXtraField(lay, &back);
}
bool DoubleSidedCard::canParse(const QString& header) {
    return header == "d";
}
SideXtra parseSide(const QString& txt) {
    QStringList spl = txt.split("|");

    if (spl.size() != 5) {
        qFatal() << "Side does not contain exactly 5 parts!";
    }

    return {unSafe(spl[0]), unSafe(spl[1]), unSafe(spl[2]), unSafe(spl[3]), unSafe(spl[4])};
}
BaseCardTyp* DoubleSidedCard::parse(const QString& header, QTextStream& in) {
    QString frtxt = tryReadLine(in, "DoubleSidedCard needs a front, but not provided!");
    QString bktxt = tryReadLine(in, "DoubleSidedCard needs a back, but not provided!");
    return new DoubleSidedCard(parseSide(frtxt), parseSide(bktxt));
}
void DoubleSidedCard::toFile(QTextStream& out) {
    out << "d\n"
        << front.forCard() << "\n"
        << back.forCard() << "\n";
}

