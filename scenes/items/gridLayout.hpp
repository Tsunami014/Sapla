#pragma once
#include <vector>
#include <QString>
#include <QGraphicsItem>
#include <QSvgRenderer>

struct layout {
    int8_t topWid;
    int8_t botWid;
    int8_t botOffs;
    QString fname;
};

struct gridItem {
    QGraphicsItem* item;
    uint8_t x;
    uint8_t y;
    layout lay;
};

class GLayoutGraphicItem : public QGraphicsItem {
public:
    GLayoutGraphicItem(QGraphicsItem* parent = nullptr);

    bool addItem(QGraphicsItem* item, layout lay);
    void setRect(const QRectF& newRect);

protected:
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

private:
    QRectF rect{0, 0, 800, 600}; // default size, will be overridden
    std::vector<gridItem> grid;
    inline static std::unique_ptr<QSvgRenderer> MTrenderer;

    inline static int Cols = 3;
};
