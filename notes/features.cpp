#include "features.hpp"
#include "../log.hpp"
#include <QRegularExpression>

const QString MODULE = "CardFeature";
constexpr auto MO = QRegularExpression::MultilineOption;

INIT_FEAT(SingleSideFeat)

void registerNoteFeatures() {
    REGISTER_FEAT(SingleSideFeat);
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
    QStringList parts = txt.split("---", Qt::SkipEmptyParts);
    l.emplace_back(parent, parts[0], parts[1]);
    return l;
}
QString SingleSideFeat::markup(QString& line) {
    if (QRegularExpression(R"(\s*---\s*)").match(line).hasMatch()) {
        return "───";
    }
    return line;
}

