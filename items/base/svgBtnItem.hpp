#pragma once
#include "svgItem.hpp"
#include "markdown.hpp"

class SvgBtnItem : public SvgGraphicItem {
public:
    SvgBtnItem(const QString& svgPath, QGraphicsItem* parent = nullptr);
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;


    std::function<void()> onClick;

protected:
    bool hovering;
};

class TxtBtnItem : public SvgBtnItem {
public:
    TxtBtnItem(const QString& svgPath, const QString& text, QGraphicsItem* parent = nullptr);
    TxtBtnItem(const QString& svgPath, QGraphicsItem* parent = nullptr);

    void setRect(const QRectF& newRect) override;
    void setPos(const QPointF& pos);

    void setText(const QString& text);  // This mucks up the pos, would not advise using unless you know you will update the rect/pos after
    void setTxtColour(const QColor& colour);
    void setFont(const QFont& font);  // This mucks up the pos, would not advise using unless you know you will update the rect/pos after

protected:
    MarkdownGraphicsText* txt;
};

