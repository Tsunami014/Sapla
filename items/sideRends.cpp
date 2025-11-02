#include "sideRends.hpp"
#include "../base/markdown.hpp"
#include "../base/font.hpp"
#include <QTextDocument>

#define EQUALS_BEGIN(DerivedType, other) \
    if (typeid(*this) != typeid(other)) return false; \
    const DerivedType& o = static_cast<const DerivedType&>(other);

void TextSide::render(QPainter* painter, QRectF rect) {
    rect.setX(rect.x() + rect.width()*0.1);
    rect.setWidth(rect.width()*0.8);
    rect.setY(rect.y() + rect.height()*0.1);
    rect.setHeight(rect.height()*0.8);

    // Create a text document
    QTextDocument doc;
    doc.setHtml(parseMarkdownHtml(text));
    // Set correct font
    doc.setDefaultFont(getFont(1.5));
    // Set correct colour
    QTextCursor curs(&doc);
    curs.select(QTextCursor::Document);
    QTextCharFormat fmt;
    fmt.setForeground(Qt::black);
    curs.mergeCharFormat(fmt);
    // Make it centred horizontally
    doc.setTextWidth(rect.width());
    QTextOption opt = doc.defaultTextOption();
    opt.setAlignment(Qt::AlignHCenter);
    doc.setDefaultTextOption(opt);
    // Make it centred vertically
    qreal hei = doc.size().height();
    qreal yOffs = rect.top() + (rect.height() - hei) / 2.0;
    // Draw document
    painter->save();
    painter->translate(rect.left(), yOffs);
    doc.drawContents(painter);
    painter->restore();
}
void TextSide::renderSelected(QPainter* painter, QRectF rect) {
    TextSide::render(painter, rect);
}
bool TextSide::operator==(const BaseSideRend& other) const {
    EQUALS_BEGIN(TextSide, other)
    return o.text == text;
}

