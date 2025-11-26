#include "features.hpp"
#include "../log.hpp"
#include <QRegularExpression>

const QString MODULE = "CardFeature";
constexpr auto MO = QRegularExpression::MultilineOption;

const QString grey = "style='color:#D5D0D5;'";

void registerNoteFeatures() {
    REGISTER_CFEAT(SingleSideFeat);
    REGISTER_CFEAT(DoubleSideFeat);
    REGISTER_FEAT(TemplateFeat);
    REGISTER_FEAT(HiddenFeat);

    std::stable_sort(Feats.begin(), Feats.end(), [](const std::unique_ptr<FeatReg>& a, const std::unique_ptr<FeatReg>& b) {
        return a->order() < b->order();
    });
}

QString trimNL(const QString& orig) {
    QString nstr = orig;
    static const QRegularExpression re(R"(^\n+|\n+$)");
    return nstr.remove(re);
}


const QRegularExpression templDefRe(R"(^ *%\|\s*([^|%\n ]+) *[ |\n] *((?:.|\n)*?)\s*\|% *$)", QRegularExpression::MultilineOption);
const QRegularExpression templApplyRe(R"(%%\s*([^%\n ]+)\s*(?:[ |\n]\s*(.*?)(?:\s*[|\n]\s*(.*?))*?)?\s*%%)", QRegularExpression::MultilineOption);
QString TemplateFeat::replacements(QString& txt, Side s) const {
    return txt.replace(templApplyRe, "");
}
QString TemplateFeat::markup(QString& line) const {
    static const QRegularExpression re(R"((%[%|]|\|%))");
    return line.replace(re, QString("<b %1>\\1</b>").arg(grey));
}
std::vector<BtnFeatures> TemplateFeat::btns() const {
    return {
           {"%||%", "%|$CUR$|%", std::nullopt, "Template definition",
            "Defines a note template\n"
            "See this screen help for more info"
        }, {"%%", "%%$CUR$%%", std::nullopt, "Template usage",
            "Uses a template for the note\n"
            "See this screen help for more info"
        }
    };
}

const QRegularExpression ssfRe("^ *--- *$", MO);
QString SingleSideFeat::markup(QString& line) const {
    if (ssfRe.match(line).hasMatch()) {
        return "───";
    }
    return line;
}
bool SingleSideFeat::dominance(const QString& txt) const {
    return ssfRe.match(txt).hasMatch();
}
FeatReturnTyp SingleSideFeat::getFlashCards(Note* parent, const QString& txt) const {
    int amnt = txt.count(ssfRe);
    if (amnt == 0) return std::nullopt;
    if (amnt != 1) {
        parent->error += "Found multiple `---` - there should only be 1!\n";
        return std::nullopt;
    }
    std::vector<FlashCard> l{};
    QStringList parts = txt.split("---");
    l.emplace_back(parent, trimNL(parts[0]), trimNL(parts[1]));
    return l;
}
std::vector<BtnFeatures> SingleSideFeat::btns() const {
    return {{"---", "\n---\n", std::nullopt, "Single sided note", 
        "Separates the card into a front and a back.\n"
        "This creates a single sided card, where the front is above the line and the back is below."
    }};
}

const QRegularExpression dsfRe("^ *=== *$", MO);
QString DoubleSideFeat::markup(QString& line) const {
    if (dsfRe.match(line).hasMatch()) {
        return "═══";
    }
    return line;
}
bool DoubleSideFeat::dominance(const QString& txt) const {
    return dsfRe.match(txt).hasMatch();
}
FeatReturnTyp DoubleSideFeat::getFlashCards(Note* parent, const QString& txt) const {
    int amnt = txt.count(dsfRe);
    if (amnt == 0) return std::nullopt;
    if (amnt != 1) {
        parent->error += "Found multiple `===` - there should only be 1!";
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
std::vector<BtnFeatures> DoubleSideFeat::btns() const {
    return {{"===", "\n===\n", std::nullopt, "Double sided note", 
        "Separates the card into 2 sides.\n"
        "This creates 2 cards: one where the top is on the front and the bottom is on the back, and another the other way around."
    }};
}


const QRegularExpression hiddenRe(R"(\[\[((?:.|\n)*?)(?<!\\):((?:.|\n)*?)\]\])");
QString HiddenFeat::replacements(QString& txt, Side s) const {
    QString repl = s == SIDE_BACK ? "\\2" : "\\1";
    return txt.replace(hiddenRe, repl);
}
QString HiddenFeat::markup(QString& line) const {
    return line.replace(hiddenRe, QString("<b %1>[[</b>\\1<span %1>:</span>\\2<b %1>]]</b>").arg(grey));
}
std::vector<BtnFeatures> HiddenFeat::btns() const {
    return {{"[[:]]", "[[$CUR$:]]", std::nullopt, "Hidden sides", 
        "Only shows text if it's on a specific side\n"
        "E.g. `[[hi:bye]]` would show hi if it's on the front side of a flashcard and bye if it's on the back\n"
        "Leaving sides blank is ok (e.g. `[[front:]]`)"
    }};
}

