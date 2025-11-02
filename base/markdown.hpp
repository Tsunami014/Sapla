#pragma once
#include <QTextEdit>
#include <QGraphicsTextItem>

QString parseMarkdownHtml(QString txt);

class MarkdownEdit : public QTextEdit {
public:
    MarkdownEdit(QWidget* parent = nullptr);
    MarkdownEdit(const QString& text, QWidget* parent = nullptr);
    void setMarkdown(const QString& text);
    QString getMarkdown();

    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void focusOutEvent(QFocusEvent *event) override;
protected:
    void updateTxt(bool save, bool orig);
private:
    void init();
};

class MarkdownGraphicsText : public QGraphicsTextItem {
public:
    MarkdownGraphicsText(QGraphicsItem* parent = nullptr);
    MarkdownGraphicsText(const QString& text, QGraphicsItem* parent = nullptr);
    void setMarkdown(const QString& text);
};

