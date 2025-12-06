#pragma once
#include <QString>
#include <QRegularExpression>
#include "note.hpp"
#include "../wids/topbar.hpp"

class _AutoColour {
public:
    _AutoColour() : idx(0) {}
    QString nxtCol();
private:
    int idx;
};
inline _AutoColour AC;

struct FeatReg {
    virtual void init() {}
    virtual int order() const { return 0; }
    virtual const QString getName() const = 0;
    virtual std::vector<BtnFeatures> btns() const { return {}; }
    virtual QString replacements(QString& txt, Side s) const { return txt; }
    virtual QString markup(QString& line) const { return line; }
};
struct CardFeatReg : public FeatReg {
    virtual std::vector<std::unique_ptr<FlashCard>> getFlashCards(Note* parent, const QString& txt, std::map<int, Schedule> schds) const { return {}; }
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

#define Feat_useCol        void init() override { col = AC.nxtCol(); } QString col
#define Feat_useCols(amnt) void init() override { for (int i=0; i<amnt; i++) { cols.push_back(AC.nxtCol()); } } std::vector<QString> cols
#define Feat_order(ordr)   int order() const override { return ordr; }
#define Feat_name(nam)     inline const QString getName() const override { return nam; }

#define Feat_btns          std::vector<BtnFeatures> btns() const override
#define Feat_replacements  QString replacements(QString& txt, Side s) const override
#define Feat_markup        QString markup(QString& line) const override

#define Feat_getFlashCards std::vector<std::unique_ptr<FlashCard>> getFlashCards(Note* parent, const QString& txt, std::map<int, Schedule> schds) const override
#define Feat_dominance     bool dominance(const QString& txt) const override


extern const QRegularExpression templDefRe;
extern const QRegularExpression templApplyRe;
extern const QRegularExpression noteInfRe;
extern const QRegularExpression scheduleInfRe;
struct BuiltInFeats : FeatReg {
    Feat_useCols(4);
    Feat_order(99)
    Feat_name("BI");
    Feat_replacements;
    Feat_markup;
    Feat_btns;
};

struct SingleSideFeat : CardFeatReg {
    Feat_useCol;
    Feat_order(2)
    Feat_name("---");
    Feat_markup;
    Feat_dominance;
    Feat_getFlashCards;
    Feat_btns;
};
struct DoubleSideFeat : CardFeatReg {
    Feat_useCol;
    Feat_order(2)
    Feat_name("===");
    Feat_markup;
    Feat_dominance;
    Feat_getFlashCards;
    Feat_btns;
};

struct HiddenFeat : FeatReg {
    Feat_useCol;
    Feat_name("[[:]]");
    Feat_replacements;
    Feat_markup;
    Feat_btns;
};

