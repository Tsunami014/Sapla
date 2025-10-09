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
    void toFile(QTextStream& out) override;

#define REGISTER_CARD(cls) \
    CardRegistry::registry.push_back({&cls::canParse, &cls::parse});


class BaseCardTyp {
public:
    virtual ~BaseCardTyp() {}
    virtual void createForm(QVBoxLayout* lay, QTreeWidgetItem* trit) = 0;
    virtual CardGraphicItem* getItem() const = 0;
    virtual QString getName() = 0;
    static bool canParse(const QString& header);
    static BaseCardTyp* parse(const QString& header, QTextStream& in);
    virtual void toFile(QTextStream& out) = 0;
    bool operator==(const BaseCardTyp& other) const;
    bool operator==(const CardGraphicItem& other) const;
};

void registerCardTypes();
namespace CardRegistry {
    using CanParseFunc = bool (*)(const QString&);
    using ParseFunc = BaseCardTyp* (*)(const QString& header, QTextStream& in);
    struct Entry {
        CanParseFunc canParse;
        ParseFunc parse;
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

