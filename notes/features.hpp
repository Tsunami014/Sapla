#pragma once
#include <QString>
#include "note.hpp"
#include "../wids/topbar.hpp"

using FeatReturnTyp = std::optional<std::vector<FlashCard>>;
struct FeatRegistry {
    virtual FeatReturnTyp getFlashCards(Note* parent, const QString& txt) const { return std::nullopt; }
    virtual QString replacements(QString& txt, Side s) const { return txt; }
    virtual QString markup(QString& line) const { return line; }
    virtual std::vector<BtnFeatures> btns() const = 0;
};

inline std::vector<std::unique_ptr<FeatRegistry>> Feats;
#define REGISTER_FEAT(nam) Feats.push_back(std::make_unique<nam>())
void registerNoteFeatures();

#define Feat_getFlashCards FeatReturnTyp getFlashCards(Note* parent, const QString& txt) const override
#define Feat_replacements  QString replacements(QString& txt, Side s) const override
#define Feat_markup        QString markup(QString& line) const override
#define Feat_btns          std::vector<BtnFeatures> btns() const override


struct SingleSideFeat : FeatRegistry {
    Feat_getFlashCards;
    Feat_markup;
    Feat_btns;
};
struct DoubleSideFeat : FeatRegistry {
    Feat_getFlashCards;
    Feat_markup;
    Feat_btns;
};

struct HiddenFeat : FeatRegistry {
    Feat_replacements;
    Feat_markup;
    Feat_btns;
};

