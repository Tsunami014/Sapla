#include "features.hpp"
#include "../log.hpp"
#include <QRegularExpression>

const QString MODULE = "CardFeature";
constexpr auto MO = QRegularExpression::MultilineOption;

void registerNoteFeatures() {
    REGISTER_FEAT(SingleSideFeat);
    REGISTER_FEAT(DoubleSideFeat);
    REGISTER_FEAT(HiddenFeat);
}

QString trimNL(const QString& orig) {
    QString nstr = orig;
    return nstr.remove(QRegularExpression(R"(^\n+|\n+$)"));
}



FeatReturnTyp SingleSideFeat::getFlashCards(Note* parent, const QString& txt) const {
    int amnt = txt.count(QRegularExpression("^ *--- *$", MO));
    if (amnt == 0) return std::nullopt;
    if (amnt != 1) {
        Log::Warn(MODULE) << "Found multiple `---` - there should only be 1!";
        return std::nullopt;
    }
    std::vector<FlashCard> l{};
    QStringList parts = txt.split("---");
    l.emplace_back(parent, trimNL(parts[0]), trimNL(parts[1]));
    return l;
}
bool SingleSideFeat::dominance(const QString& txt) const {
    return QRegularExpression(QString("^ *%1 *$").arg(getName()), MO).match(txt).hasMatch();
}
QString SingleSideFeat::markup(QString& line) const {
    if (QRegularExpression(" *--- *").match(line).hasMatch()) {
        return "───";
    }
    return line;
}
std::vector<BtnFeatures> SingleSideFeat::btns() const {
    return {{"---", "\n---\n", std::nullopt, "Single sided note", 
        "Separates the card into a front and a back.\n"
        "This creates a single sided card, where the front is above the line and the back is below."
    }};
}

FeatReturnTyp DoubleSideFeat::getFlashCards(Note* parent, const QString& txt) const {
    int amnt = txt.count(QRegularExpression("^ *=== *$", MO));
    if (amnt == 0) return std::nullopt;
    if (amnt != 1) {
        Log::Warn(MODULE) << "Found multiple `===` - there should only be 1!";
        return std::nullopt;
    }
    std::vector<FlashCard> l{};
    QStringList parts = txt.split("===");
    QString first = trimNL(parts[0]);
    QString second = trimNL(parts[1]);
    l.emplace_back(parent, first, second);
    l.emplace_back(parent, second, first);
    return l;
}
QString DoubleSideFeat::markup(QString& line) const {
    if (QRegularExpression(" *=== *").match(line).hasMatch()) {
        return "═══";
    }
    return line;
}
std::vector<BtnFeatures> DoubleSideFeat::btns() const {
    return {{"===", "\n===\n", std::nullopt, "Double sided note", 
        "Separates the card into 2 sides.\n"
        "This creates 2 cards: one where the top is on the front and the bottom is on the back, and another the other way around."
    }};
}


QString HiddenFeat::replacements(QString& txt, Side s) const {
    QString repl;
    if (s == SIDE_BACK) {
        repl = R"([^:\[\]]*:([^:\[\]]*))";
    } else {
        repl = R"(([^:\[\]]*):[^:\[\]]*)";
    }
    auto re = QRegularExpression(QString(R"(\[\[%1\]\])").arg(repl));
    return txt.replace(re, "\\1");
}
QString HiddenFeat::markup(QString& line) const {
    auto re = QRegularExpression(R"(\[\[([^\[\]:]*):([^\[\]:]*)\]\])");
    QString grey = "style='color:#D5D0D5;'";
    return line.replace(re, QString("<b %1>[[</b>\\1<span %1>:</span>\\2<b %1>]]</b>").arg(grey));
}
std::vector<BtnFeatures> HiddenFeat::btns() const {
    return {{"[[:]]", "[[$CUR$:]]", std::nullopt, "Hidden sides", 
        "Only shows text if it's on a specific side\n"
        "E.g. `[[hi:bye]]` would show hi if it's on the front side of a flashcard and bye if it's on the back\n"
        "Leaving sides blank is ok (e.g. `[[front:]]`)"
    }};
}

