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

    void textXtraField(QVBoxLayout* lay, const SideXtra& init, std::function<void(int, const QString&)> onChange) {
        QHBoxLayout* lay2 = new QHBoxLayout();

        auto makeTextEdit = [&](const QString& text, int stretch, int index) {
            QTextEdit* edit = new QTextEdit(text);
            lay2->addWidget(edit, stretch);
            if (onChange) {
                QObject::connect(edit, &QTextEdit::textChanged, [edit, onChange, index]() {
                    onChange(index, edit->toPlainText());
                });
            }
            return edit;
        };

        QTextEdit* prefixEdit = makeTextEdit(init.prefix, 1, 0);
        QTextEdit* txtEdit    = makeTextEdit(init.txt,    3, 1);
        QTextEdit* suffixEdit = makeTextEdit(init.suffix, 1, 2);

        lay->addLayout(lay2);
    }

}

