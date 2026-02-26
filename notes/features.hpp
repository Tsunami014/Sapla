#pragma once
#include <QString>
#include <QRegularExpression>
#include "note.hpp"

class _AutoColour {
public:
    _AutoColour() : idx(0) {}
    QString nxtCol();
private:
    int idx;
};
inline _AutoColour AC;

#define FeatDef(nam) \
    inline static std::unique_ptr<nam> instance = std::make_unique<nam>();

struct FeatReg {
    inline static std::unique_ptr<FeatReg> instance = nullptr;
    virtual void init() {}
    virtual const QString getName() const = 0;
    virtual int order() const { return 0; }
    QString highersReplace(QString inp);
    virtual QMap<QString, QString> help() const { return {}; }
    virtual QString replacements(QString& txt, Side s) const { return txt; }
    virtual QString markup(QString& line) const { return line; }
};
struct CardFeatReg : public FeatReg {
    inline static std::unique_ptr<CardFeatReg> instance = nullptr;
    virtual std::vector<std::unique_ptr<FlashCard>> getFlashCards(Note* parent, const QString& txt, std::map<int, Schedule> schds) const { return {}; }
    virtual bool dominance(const QString& txt) const { return false; }
};

inline std::vector<FeatReg*> Feats;
inline std::vector<CardFeatReg*> CardFeats;
#define REGISTER_FEAT(nam) \
    Feats.push_back(nam::instance.get())
#define REGISTER_CFEAT(nam) \
    CardFeats.push_back(nam::instance.get());\
    Feats.push_back(nam::instance.get());

void registerNoteFeatures();
Schedule getSchd(std::map<int, Schedule> schds, int idx);

#define Feat_useCol        void init() override { col = AC.nxtCol(); } QString col
#define Feat_useCols(amnt) void init() override { for (int i=0; i<amnt; i++) { cols.push_back(AC.nxtCol()); } } std::vector<QString> cols
#define Feat_order(ordr)   int order() const override { return ordr; }
#define Feat_name(nam)     inline const QString getName() const override { return nam; }

#define Feat_help          QMap<QString, QString> help() const override
#define Feat_replacements  QString replacements(QString& txt, Side s) const override
#define Feat_markup        QString markup(QString& line) const override

#define Feat_getFlashCards std::vector<std::unique_ptr<FlashCard>> getFlashCards(Note* parent, const QString& txt, std::map<int, Schedule> schds) const override
#define Feat_dominance     bool dominance(const QString& txt) const override


extern const QRegularExpression scheduleInfRe;
struct ScheduleFeat : FeatReg {
    FeatDef(ScheduleFeat)
    Feat_order(1000);
    Feat_name("<<>>");
    Feat_replacements;
    Feat_markup;
    Feat_help;
};
extern const QRegularExpression templDefRe;
extern const QRegularExpression templLoclDefRe;
extern const QRegularExpression templApplyRe;
struct TemplateFeat : FeatReg {
    FeatDef(TemplateFeat)
    Feat_useCols(2);
    Feat_order(100);
    Feat_name("|| ||");
    Feat_replacements;
    Feat_markup;
    Feat_help;
};
extern const QRegularExpression noteInfRe;
struct TagFeat : FeatReg {
    FeatDef(TagFeat)
    Feat_useCol;
    Feat_order(90);
    Feat_name("#tag");
    Feat_replacements;
    Feat_markup;
    Feat_help;
};

struct SingleSideFeat : CardFeatReg {
    FeatDef(SingleSideFeat)
    Feat_useCol;
    Feat_order(2);
    Feat_name("---");
    Feat_dominance;
    Feat_getFlashCards;
    Feat_replacements;
    Feat_markup;
    Feat_help;
};
struct DoubleSideFeat : CardFeatReg {
    FeatDef(DoubleSideFeat)
    Feat_useCol;
    Feat_order(2);
    Feat_name("===");
    Feat_dominance;
    Feat_getFlashCards;
    Feat_replacements;
    Feat_markup;
    Feat_help;
};

struct SecretFeat : CardFeatReg {
    FeatDef(SecretFeat)
    Feat_useCol;
    Feat_order(1);
    Feat_name("{ }");
    Feat_getFlashCards;
    Feat_replacements;
    Feat_markup;
    Feat_help;
};

struct HiddenFeat : FeatReg {
    FeatDef(HiddenFeat)
    Feat_useCol;
    Feat_order(110);
    Feat_name("[[ ]]");
    Feat_replacements;
    Feat_markup;
    Feat_help;
};

