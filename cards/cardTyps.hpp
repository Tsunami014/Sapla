#pragma once
#include "../items/cardIt.hpp"
#include <QGraphicsItem>

#define INIT_CARD \
public:\
    CardGraphicItem* getItem() const override;\
    QString getName() override;

class BaseCardTyp {
public:
    virtual ~BaseCardTyp() {}
    virtual CardGraphicItem* getItem() const = 0;
    virtual QString getName() = 0;
    bool operator==(const BaseCardTyp& other) const;
    bool operator==(const CardGraphicItem& other) const;
};

class TextCard : public BaseCardTyp {
    INIT_CARD
public:
    TextCard(QString front, QString back);
protected:
    QString front;
    QString back;
};

