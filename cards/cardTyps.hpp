#pragma once
#include "../items/cardIt.hpp"
#include <QGraphicsItem>
#include <QVBoxLayout>
#include <QTreeWidgetItem>

#define INIT_CARD \
public:\
    void createForm(QVBoxLayout* lay, QTreeWidgetItem* trit) override;\
    CardGraphicItem* getItem() const override;\
    QString getName() override;\
    static bool canParse(const QString& header);\
    static BaseCardTyp* parse(const QString& header, QTextStream& in);\
    static BaseCardTyp* newBlank();\
    void toFile(QTextStream& out) override;

#define REGISTER_CARD(cls, name) \
    CardRegistry::registry.push_back({name, &cls::canParse, &cls::parse, &cls::newBlank});


class BaseCardTyp {
public:
    virtual ~BaseCardTyp() {}
    virtual void createForm(QVBoxLayout* lay, QTreeWidgetItem* trit) = 0;
    virtual CardGraphicItem* getItem() const = 0;
    virtual QString getName() = 0;
    static bool canParse(const QString& header);
    static BaseCardTyp* parse(const QString& header, QTextStream& in);
    virtual void toFile(QTextStream& out) = 0;
    static BaseCardTyp* newBlank();
    bool operator==(const BaseCardTyp& other) const;
    bool operator==(const CardGraphicItem& other) const;
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
    INIT_CARD
public:
    TextCard(QString front, QString back);
protected:
    QString front;
    QString back;
};

