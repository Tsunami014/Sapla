#pragma once
#include "base/rectIt.hpp"
#include <QGraphicsProxyWidget>
#include <QListWidget>

class TextListItem : public QGraphicsProxyWidget, public RectMixin {
public:
    TextListItem(QGraphicsItem* parent = nullptr);
    QRectF boundingRect() const override;
    void setRect(const QRectF& newRect) override;

    QListWidget* list;
};
