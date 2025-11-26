#pragma once
#include <QString>
#include <QRegularExpression>
#include "note.hpp"
#include "../wids/topbar.hpp"

using FeatReturnTyp = std::optional<std::vector<FlashCard>>;
struct FeatReg {
    virtual int order() const { return 0; }
    virtual const QString getName() const = 0;
    virtual std::vector<BtnFeatures> btns() const { return {}; }
    virtual QString replacements(QString& txt, Side s) const { return txt; }
    virtual QString markup(QString& line) const { return line; }
};
struct CardFeatReg : public FeatReg {
    virtual FeatReturnTyp getFlashCards(Note* parent, const QString& txt) const { return std::nullopt; }
    virtual bool dominance(const QString& txt) const { return false; }
};

inline std::vector<std::unique_ptr<FeatReg>> Feats;
inline std::vector<CardFeatReg*> CardFeats;
#define REGISTER_FEAT(nam) Feats.push_back(std::make_unique<nam>())
#define REGISTER_CFEAT(nam) {\
    auto ptr = std::make_unique<nam>();\
    CardFeats.push_back(ptr.get());\
    Feats.push_back(std::move(ptr));\
}
void registerNoteFeatures();

#define Feat_order(ordr)   int order() const override { return ordr; }
#define Feat_name          inline const QString getName() const override
#define Feat_replacements  QString replacements(QString& txt, Side s) const override
#define Feat_markup        QString markup(QString& line) const override
#define Feat_dominance     bool dominance(const QString& txt) const override
#define Feat_getFlashCards FeatReturnTyp getFlashCards(Note* parent, const QString& txt) const override
#define Feat_btns          std::vector<BtnFeatures> btns() const override


extern const QRegularExpression templDefRe;
extern const QRegularExpression templApplyRe;
struct TemplateFeat : FeatReg {
    Feat_order(-999)
    Feat_name { return "%%"; };
    Feat_replacements;
    Feat_markup;
    Feat_btns;
};

struct SingleSideFeat : CardFeatReg {
    Feat_name { return "---"; };
    Feat_markup;
    Feat_dominance;
    Feat_getFlashCards;
    Feat_btns;
};
struct DoubleSideFeat : CardFeatReg {
    Feat_name { return "==="; };
    Feat_markup;
    Feat_dominance;
    Feat_getFlashCards;
    Feat_btns;
};

struct HiddenFeat : FeatReg {
    Feat_name { return "[[:]]"; };
    Feat_replacements;
    Feat_markup;
    Feat_btns;
};

