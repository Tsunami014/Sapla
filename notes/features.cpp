#include "features.hpp"
#include "../log.hpp"
#include <QRegularExpression>

const QString MODULE = "CardFeature";
constexpr auto MO = QRegularExpression::MultilineOption;

INIT_FEAT(SingleSideFeat)
INIT_FEAT(DoubleSideFeat)

void registerNoteFeatures() {
    REGISTER_FEAT(SingleSideFeat);
    REGISTER_FEAT(DoubleSideFeat);
}

QString trimNL(const QString& orig) {
    QString nstr = orig;
    return nstr.remove(QRegularExpression(R"(^\n+|\n+$)"));
}


FeatReturnTyp SingleSideFeat::getFlashCards(Note* parent, const QString& txt) {
    int amnt = txt.count(QRegularExpression(R"(^\s*---\s*$)", MO));
    if (amnt == 0) return std::nullopt;
    if (amnt != 1) {
        Log::Warn(MODULE) << "Found multiple `---` - there should only be 1!";
        return std::nullopt;
    }
    if (QRegularExpression(R"(^\s*===\s*$)", MO).match(txt).hasMatch()) {
        Log::Warn(MODULE) << "Found both `---` and `===` in the same card!";
        return std::nullopt;
    }
    std::vector<FlashCard> l{};
    QStringList parts = txt.split("---");
    l.emplace_back(parent, trimNL(parts[0]), trimNL(parts[1]));
    return l;
}
QString SingleSideFeat::markup(QString& line) {
    if (QRegularExpression(R"(\s*---\s*)").match(line).hasMatch()) {
        return "───";
    }
    return line;
}
std::vector<BtnFeatures> SingleSideFeat::btns() {
    return {{"---", "\n---\n$CURS$", std::nullopt, "Single sided note", 
        "Separates the card into a front and a back.\n"
        "This creates a single sided card, where the front is above the line and the back is below."
    }};
}

FeatReturnTyp DoubleSideFeat::getFlashCards(Note* parent, const QString& txt) {
    int amnt = txt.count(QRegularExpression(R"(^\s*===\s*$)", MO));
    if (amnt == 0) return std::nullopt;
    if (amnt != 1) {
        Log::Warn(MODULE) << "Found multiple `===` - there should only be 1!";
        return std::nullopt;
    }
    if (QRegularExpression(R"(^\s*---\s*$)", MO).match(txt).hasMatch()) {
        Log::Warn(MODULE) << "Found both `---` and `===` in the same card!";
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
QString DoubleSideFeat::markup(QString& line) {
    if (QRegularExpression(R"(\s*===\s*)").match(line).hasMatch()) {
        return "═══";
    }
    return line;
}
std::vector<BtnFeatures> DoubleSideFeat::btns() {
    return {{"===", "\n===\n$CURS$", std::nullopt, "Double sided note", 
        "Separates the card into 2 sides.\n"
        "This creates 2 cards: one where the top is on the front and the bottom is on the back, and another the other way around."
    }};
}

