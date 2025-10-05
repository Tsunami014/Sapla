#pragma once
// I got sick of writing the same values again and again
#include <QGraphicsItem>

class MyGraphicsItem : public QGraphicsItem {
public:
    MyGraphicsItem(QGraphicsItem* parent = nullptr);
    virtual void setRect(const QRectF& newRect);
    QRectF boundingRect() const override;
protected:
    QRectF rect{0, 0, 800, 600}; // default size, will be overridden
};

