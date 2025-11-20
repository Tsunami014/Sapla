#pragma once
#include <QString>
#include <QRegularExpression>
#include "note.hpp"
#include "../wids/topbar.hpp"

using FeatReturnTyp = std::optional<std::vector<FlashCard>>;
struct FeatRegistry {
    const int Order = 0;
    virtual const QString getName() const = 0;
    virtual FeatReturnTyp getFlashCards(Note* parent, const QString& txt) const { return std::nullopt; }
    virtual bool dominance(const QString& txt) const { return false; }
    virtual QString replacements(QString& txt, Side s) const { return txt; }
    virtual QString markup(QString& line) const { return line; }
    virtual std::vector<BtnFeatures> btns() const { return {}; }
};

inline std::vector<std::unique_ptr<FeatRegistry>> Feats;
#define REGISTER_FEAT(nam) Feats.push_back(std::make_unique<nam>())
void registerNoteFeatures();

#define Feat_name          inline const QString getName() const override
#define Feat_getFlashCards FeatReturnTyp getFlashCards(Note* parent, const QString& txt) const override
#define Feat_dominance     bool dominance(const QString& txt) const override
#define Feat_replacements  QString replacements(QString& txt, Side s) const override
#define Feat_markup        QString markup(QString& line) const override
#define Feat_btns          std::vector<BtnFeatures> btns() const override


extern const QRegularExpression templDefRe;
extern const QRegularExpression templApplyRe;
struct TemplateFeat : FeatRegistry {
    const int Order = -999;
    Feat_name { return "%%"; };
    Feat_dominance;
    Feat_replacements;
    Feat_btns;
};

struct SingleSideFeat : FeatRegistry {
    Feat_name { return "---"; };
    Feat_getFlashCards;
    Feat_dominance;
    Feat_markup;
    Feat_btns;
};
struct DoubleSideFeat : FeatRegistry {
    Feat_name { return "==="; };
    Feat_getFlashCards;
    Feat_dominance;
    Feat_markup;
    Feat_btns;
};

struct HiddenFeat : FeatRegistry {
    Feat_name { return "[[:]]"; };
    Feat_replacements;
    Feat_markup;
    Feat_btns;
};

