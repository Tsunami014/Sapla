#include "formElms.hpp"
#include <QLabel>
#include <QTextEdit>

namespace Form {

    void labelField(QVBoxLayout* lay, const QString& txt) {
        lay->addWidget(new QLabel(txt));
    }

    void textField(QVBoxLayout* lay, const QString& initTxt, std::function<void(const QString&)> onChange) {
        QTextEdit* edit = new QTextEdit(initTxt);
        lay->addWidget(edit);

        if (onChange) {
            QObject::connect(edit, &QTextEdit::textChanged, [edit, onChange](){
                onChange(edit->toPlainText());
            });
        }
    }

}

