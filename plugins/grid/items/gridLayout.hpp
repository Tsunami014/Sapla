#pragma once
#include "../game.hpp"
#include "cardIt.hpp"
#include <vector>
#include <QString>
#include <QSvgRenderer>

class CardGraphicItem;  // Forward declaration

struct layout {
    int8_t topWid;
    int8_t botWid;
    int8_t botOffs;
    QString fname;
};

struct gridItem {
    GridCGI* item;
    uint8_t x;
    uint8_t y;
    layout lay;
};

class GLayoutGraphicItem : public RectItem {
public:
    GLayoutGraphicItem(QGraphicsItem* parent = nullptr);

    void updateAllChildren();

    bool addItem(GridCGI* item, layout& lay);
    void removeItem(GridCGI* item);

    void setRect(const QRectF& newRect) override;
    QRectF getRealRect();

    std::vector<gridItem> grid;
    inline static int Cols = 2;
protected:
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

    bool isGood(int col, int row);

    inline static std::unique_ptr<QSvgRenderer> MTrenderer;
};
