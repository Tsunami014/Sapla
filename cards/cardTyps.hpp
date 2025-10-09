#pragma once
#include "../items/cardIt.hpp"
#include <QGraphicsItem>
#include <QVBoxLayout>

#define INIT_CARD \
public:\
    void createForm(QVBoxLayout* lay) override;\
    CardGraphicItem* getItem() const override;\
    QString getName() override;

class BaseCardTyp {
public:
    virtual ~BaseCardTyp() {}
    virtual void createForm(QVBoxLayout* lay) = 0;
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

