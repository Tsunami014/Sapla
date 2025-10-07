#pragma once
#include "svgItem.hpp"

class SvgBtnItem : public SvgGraphicItem {
public:
    SvgBtnItem(const QString& svgPath, const QString& text, QGraphicsItem* parent = nullptr);
    SvgBtnItem(const QString& svgPath, QGraphicsItem* parent = nullptr);

    void setTxtColour(const QColor& colour);
    void setRect(const QRectF& newRect) override;
    void setPos(const QPointF& pos);
    void setText(const QString& text);  // This mucks up the pos, would not advise using unless you know you will update the rect/pos after
    QString getText();
    void setFont(const QFont& font);  // This mucks up the pos, would not advise using unless you know you will update the rect/pos after
protected:
    QGraphicsTextItem* txt;
};
