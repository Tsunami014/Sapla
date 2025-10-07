#pragma once
#include <QGraphicsItem>

class RectMixin {
public:
    virtual void setRect(const QRectF& newRect);
    QRectF boundingRect() const;
protected:
    QRectF rect{0, 0, 800, 600}; // default size, will be overridden
};

class RectItem : public QGraphicsItem, public RectMixin {
public:
    RectItem(QGraphicsItem* parent = nullptr);
    virtual void setRect(const QRectF& newRect) override;
    QRectF boundingRect() const override;
};

