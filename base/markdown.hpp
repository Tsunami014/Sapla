#pragma once
#include <QTextEdit>
#include <QGraphicsTextItem>
#include "../notes/features.hpp"

QString parseMarkdownHtml(QString txt);


class SaveCursor : public QObject {
public:
    SaveCursor(QTextEdit* parent);
    ~SaveCursor();
private:
    QTextEdit* edit;
    using Point = std::pair<int, int>;
    Point save(int thing);
    int load(Point saved);
    Point anchor;
    Point pos;
};

class MarkdownEdit : public QTextEdit {
    Q_OBJECT
public:
    MarkdownEdit(QWidget* parent = nullptr);
    MarkdownEdit(const QString& text, QWidget* parent = nullptr);
    void setMarkdown(const QString& text);
    QString getMarkdown();
    void setButton(bool enabled);

    void keyPressEvent(QKeyEvent* event) override;
    void inputMethodEvent(QInputMethodEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;

    void insertMarkdown(QString txt, QString cursSub = "");
    void refresh();
signals:
    void clicked();
    void altEnter();
    void altDelete();
protected:
    void updateTxt(bool save, bool orig);
    int lastCol = -1;
    bool isBtn;
private:
    void init();
};

class MarkdownGraphicsText : public QGraphicsTextItem {
public:
    MarkdownGraphicsText(QGraphicsItem* parent = nullptr);
    MarkdownGraphicsText(const QString& text, QGraphicsItem* parent = nullptr);
    void setMarkdown(const QString& text);
};

