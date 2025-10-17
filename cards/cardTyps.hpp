#pragma once
#include "../items/cardIt.hpp"
#include <QGraphicsItem>
#include <QVBoxLayout>
#include <QTreeWidgetItem>

class FlashCard;  // Forward reference

#define INIT_CARD(name) \
public:\
    inline static QString clsName = name;\
    QString typeName() const override { return clsName; }\
    void createForm(QVBoxLayout* lay, QTreeWidgetItem* trit) override;\
    CardGraphicItem* getItem(const FlashCard& fc) const override;\
    QString getName() override;\
    static bool canParse(const QString& header);\
    static BaseCardTyp* parse(const QString& header, QTextStream& in);\
    static BaseCardTyp* newBlank();\
    void toFile(QTextStream& out) override;\

#define REGISTER_CARD(cls) \
    CardRegistry::registry.push_back({cls::clsName, &cls::canParse, &cls::parse, &cls::newBlank});


class BaseCardTyp {
public:
    virtual ~BaseCardTyp() {}

    inline static QString clsName = "";
    virtual QString typeName() const { return clsName; }

    virtual void createForm(QVBoxLayout* lay, QTreeWidgetItem* trit) = 0;
    virtual CardGraphicItem* getItem(const FlashCard& fc) const = 0;
    virtual QString getName() = 0;

    static bool canParse(const QString& header);
    static BaseCardTyp* parse(const QString& header, QTextStream& in);
    virtual void toFile(QTextStream& out) = 0;
    static BaseCardTyp* newBlank();

    std::vector<FlashCard> flashCs;
};

void registerCardTypes();
namespace CardRegistry {
    using CanParseFunc = bool (*)(const QString&);
    using ParseFunc = BaseCardTyp* (*)(const QString& header, QTextStream& in);
    using NewFunc = BaseCardTyp* (*)();
    struct Entry {
        QString name;
        CanParseFunc canParse;
        ParseFunc parse;
        NewFunc newBlank;
    };
    inline std::vector<Entry> registry;
}


class TextCard : public BaseCardTyp {
    INIT_CARD("Text card")
public:
    TextCard(QString front, QString back);
protected:
    QString front;
    QString back;
};

struct SideXtra {
    QString fullTxt(bool front) const;
    QString forCard() const;
    QString fprefix;
    QString bprefix;
    QString txt;
    QString fsuffix;
    QString bsuffix;
};
class DoubleSidedCard : public BaseCardTyp {
    INIT_CARD("Text card and reverse")
public:
    DoubleSidedCard(SideXtra front, SideXtra back);
protected:
    SideXtra front;
    SideXtra back;
};

struct FlashCard {
    // This is the actual flashcard you can be shown, and contains the scheduling data and everything needed.
    const BaseCardTyp* card;
    int idx;

    CardGraphicItem* getItem() const { return card->getItem(*this); }
    bool operator==(const FlashCard& other) const;
    bool operator==(const CardGraphicItem& other) const;
};

