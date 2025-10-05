#pragma once
#include <QPainter>

#define SIDE_REND \
    void render(QPainter* painter, QRectF rect) override;\
    void renderSelected(QPainter* painter, QRectF rect) override;\
    bool operator==(const BaseSideRend& other) const override;

class BaseSideRend {
public:
    virtual ~BaseSideRend() {};
    virtual void render(QPainter* painter, QRectF rect) = 0;
    virtual void renderSelected(QPainter* painter, QRectF rect) = 0;
    virtual bool operator==(const BaseSideRend& other) const = 0;
};

class TextSide : public BaseSideRend {
public:
    SIDE_REND
    TextSide(QString txt) : text(txt) {};
private:
    QString text;
};

