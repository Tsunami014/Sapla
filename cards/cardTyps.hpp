#pragma once
#include "../items/cardIt.hpp"
#include <QGraphicsItem>

class BaseCardTyp {
public:
    virtual ~BaseCardTyp() {}
    virtual CardGraphicItem* getItem() const = 0;
    bool operator==(const BaseCardTyp& other) const;
    bool operator==(const CardGraphicItem& other) const;
};

class TextCard : public BaseCardTyp {
public:
    TextCard(QString front, QString back);
    CardGraphicItem* getItem() const override;
protected:
    QString front;
    QString back;
};

