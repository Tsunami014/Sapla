#pragma once
#include "../items/cardIt.hpp"
#include <QGraphicsItem>

class BaseCardTyp {
public:
    virtual ~BaseCardTyp() {}
    virtual CardGraphicItem* getItem() = 0;
};

class TextCard : public BaseCardTyp {
public:
    TextCard(QString front, QString back);
    CardGraphicItem* getItem() override;
protected:
    QString front;
    QString back;
};

