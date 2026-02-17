#pragma once
#include <QLineEdit>
#include <QKeyEvent>

class LineEdit2 : public QLineEdit {
    Q_OBJECT
public:
    using QLineEdit::QLineEdit;
protected:
    void keyPressEvent(QKeyEvent* event) override {
        if (event->key() == Qt::Key_Backspace && text().isEmpty()) {
            emit emptyBackspace();
            return;
        }
        QLineEdit::keyPressEvent(event);
    }
signals:
    void emptyBackspace();
};
