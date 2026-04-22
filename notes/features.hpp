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

#define Feat_Instance(nam) \
    inline static std::unique_ptr<nam> instance = std::make_unique<nam>();

struct FeatReg {
    inline static std::unique_ptr<FeatReg> instance = nullptr;
    virtual void init() {}
    virtual const QString getName() const = 0;
    virtual int order() const { return 0; }
    QString highersReplace(QString inp);
    QString othersReplace(QString inp);
    virtual QMap<QString, QString> help() const { return {}; }
    virtual QString check(QString& txt, QString& err) const { return txt; }
    virtual QString replacements(QString& txt, Side s) const { return txt; }
    virtual QString markup(QString& line) const { return line; }
};
struct CardFeatReg : public FeatReg {
    inline static std::unique_ptr<CardFeatReg> instance = nullptr;
    virtual std::vector<std::unique_ptr<FlashCard>> getFlashCards(Note* parent, const QString& txt, std::map<int, Schedule> schds, int part) const { return {}; }
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
#define Feat_check         QString check(QString& txt, QString& err) const override;
#define Feat_replacements  QString replacements(QString& txt, Side s) const override
#define Feat_markup        QString markup(QString& line) const override

#define CFeat_getFlashCards std::vector<std::unique_ptr<FlashCard>> getFlashCards(Note* parent, const QString& txt, std::map<int, Schedule> schds, int part) const override
#define CFeat_dominance     bool dominance(const QString& txt) const override


extern const QRegularExpression scheduleInfRe;
struct ScheduleFeat : FeatReg {
    Feat_Instance(ScheduleFeat)
    Feat_order(99999);
    Feat_name("<<>>");
    Feat_replacements;
    Feat_markup;
    Feat_help;
};
extern const QRegularExpression mnsRe;
struct MultinoteSepFeat : FeatReg {
    Feat_Instance(MultinoteSepFeat)
    Feat_useCol;
    Feat_order(9999);
    Feat_name("~~~");
    Feat_markup;
    Feat_help;
};
QStringList splTemplArgs(QString args);
extern const QRegularExpression templDefRe;
extern const QRegularExpression templLoclDefRe;
extern const QRegularExpression templApplyRe;
struct TemplateFeat : FeatReg {
    Feat_Instance(TemplateFeat)
    Feat_useCols(2);
    Feat_order(1000);
    Feat_name("|| ||");
    Feat_check;
    Feat_replacements;
    Feat_markup;
    Feat_help;
};
extern const QRegularExpression noteInfRe;
struct TagFeat : FeatReg {
    Feat_Instance(TagFeat)
    Feat_useCol;
    Feat_order(90);
    Feat_name("#tag");
    Feat_replacements;
    Feat_markup;
    Feat_help;
};

struct SingleSideFeat : CardFeatReg {
    Feat_Instance(SingleSideFeat)
    Feat_useCol;
    Feat_order(2);
    Feat_name("---");
    CFeat_dominance;
    CFeat_getFlashCards;
    Feat_replacements;
    Feat_markup;
    Feat_help;
};
struct DoubleSideFeat : CardFeatReg {
    Feat_Instance(DoubleSideFeat)
    Feat_useCol;
    Feat_order(2);
    Feat_name("===");
    CFeat_dominance;
    CFeat_getFlashCards;
    Feat_replacements;
    Feat_markup;
    Feat_help;
};
struct MirrorSideFeat : CardFeatReg {
    Feat_Instance(MirrorSideFeat)
    Feat_useCol;
    Feat_order(2);
    Feat_name("///");
    CFeat_dominance;
    CFeat_getFlashCards;
    Feat_replacements;
    Feat_markup;
    Feat_help;
};
struct TemplateSideFeat : FeatReg {
    Feat_Instance(TemplateSideFeat)
    Feat_useCol;
    Feat_order(1001);
    Feat_name("|||");
    Feat_check;
    Feat_replacements;
    Feat_markup;
    Feat_help;
};

struct SecretFeat : CardFeatReg {
    Feat_Instance(SecretFeat)
    Feat_useCol;
    Feat_order(1);
    Feat_name("{ }");
    CFeat_getFlashCards;
    Feat_replacements;
    Feat_markup;
    Feat_help;
};

struct HiddenFeat : FeatReg {
    Feat_Instance(HiddenFeat)
    Feat_useCol;
    Feat_order(100);
    Feat_name("[[ ]]");
    Feat_replacements;
    Feat_markup;
    Feat_help;
};

struct ScrambledFeat : FeatReg {
    Feat_Instance(ScrambledFeat)
    Feat_useCol;
    Feat_order(100);
    Feat_name(".. ..");
    Feat_replacements;
    Feat_markup;
    Feat_help;
};

struct ShuffledFeat : FeatReg {
    Feat_Instance(ShuffledFeat)
    Feat_useCol;
    Feat_order(200);
    Feat_name(">> <<");
    Feat_check;
    Feat_markup;
    Feat_help;
};
