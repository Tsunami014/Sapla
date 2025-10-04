#pragma once
#include <vector>
#include <QString>
#include <QGraphicsItem>
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

class GLayoutGraphicItem : public QGraphicsItem {
public:
    GLayoutGraphicItem(QGraphicsItem* parent = nullptr);

    bool addItem(CardGraphicItem* item);
    void removeItem(CardGraphicItem* item);
    void setRect(const QRectF& newRect);

    std::vector<gridItem> grid;
protected:
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

    QRectF rect{0, 0, 800, 600}; // default size, will be overridden
    inline static std::unique_ptr<QSvgRenderer> MTrenderer;

    inline static int Cols = 2;
};
