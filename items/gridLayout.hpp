#pragma once
#include <vector>
#include <QString>
#include "base/myItem.hpp"
#include <QSvgRenderer>

class CardGraphicItem;  // Forward declaration

struct layout {
    int8_t topWid;
    int8_t botWid;
    int8_t botOffs;
    QString fname;
};

struct gridItem {
    CardGraphicItem* item;
    uint8_t x;
    uint8_t y;
    layout lay;
};

class GLayoutGraphicItem : public MyGraphicsItem {
public:
    GLayoutGraphicItem(QGraphicsItem* parent = nullptr);

    bool addItem(CardGraphicItem* item);
    void removeItem(CardGraphicItem* item);
    void setRect(const QRectF& newRect) override;

    std::vector<gridItem> grid;
protected:
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

    bool isGood(int col, int row);

    inline static std::unique_ptr<QSvgRenderer> MTrenderer;

    inline static int Cols = 2;
};
