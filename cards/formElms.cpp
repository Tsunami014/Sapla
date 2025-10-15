#include "formElms.hpp"
#include "getCards.hpp"
#include <QLabel>
#include <QTextEdit>

namespace Form {

    void labelField(QVBoxLayout* lay, const QString& txt) {
        lay->addWidget(new QLabel(txt));
    }

    void textField(QVBoxLayout* lay, QString* initTxt, std::function<void(const QString&)> onChange, bool update) {
        QTextEdit* edit = new QTextEdit(*initTxt);
        lay->addWidget(edit);

        if (onChange) {
            QObject::connect(edit, &QTextEdit::textChanged, [edit, onChange, initTxt, update](){
                if (update) *initTxt = edit->toPlainText();
                onChange(edit->toPlainText());
            });
        }
    }

    void textXtraField(QVBoxLayout* lay, SideXtra* init, std::function<void(int, const QString&)> onChange, bool update) {
        QHBoxLayout* lay2 = new QHBoxLayout();

        auto makeTextEdit = [&](QBoxLayout* lay, QString* text, int index, bool mid = false) {
            QTextEdit* edit = new QTextEdit(*text);
            qreal heiOffs;
            int stretch;
            if (!mid) {
                heiOffs = 2.5;
                stretch = 0;
            } else {
                heiOffs = 5;
                stretch = 3;
            }
            edit->setMaximumHeight(edit->fontMetrics().height()*(heiOffs+0.2) + edit->frameWidth()*2 + 8);
            lay->addWidget(edit, stretch);
            QObject::connect(edit, &QTextEdit::textChanged, [=]() {
                if (update) *text = edit->toPlainText();
                if (onChange) onChange(index, edit->toPlainText());
                writeCards();
            });
            return edit;
        };

        QVBoxLayout* play = new QVBoxLayout();
        makeTextEdit(play, &init->fprefix, -2);
        makeTextEdit(play, &init->bprefix, -1);
        lay2->addLayout(play, 1);

        makeTextEdit(lay2, &init->txt, 0, true);

        QVBoxLayout* blay = new QVBoxLayout();
        makeTextEdit(blay, &init->fsuffix, 1);
        makeTextEdit(blay, &init->bsuffix, 2);
        lay2->addLayout(blay, 1);


        lay->addLayout(lay2);
    }

}

